/* glitch_controller
 * --------------
 * Userspace undervolting framework of VoltPillager
 *
 * Zitai Chen
 */

// sched_setaffinity need GNU_SOURCE
#define _GNU_SOURCE

#include <stdio.h>    // Standard input/output definitions
#include <stdlib.h>
#include <string.h>   // String function definitions
#include <unistd.h>   // for usleep()
#include <getopt.h>

#include <sched.h>    // for CPU schedulling
#include <assert.h>

#include "arduino-serial-lib.h"
#include <errno.h>   	 /* ERROR Number Definitions           */
#include <fcntl.h>   	 /* File Control Definitions           */
#include "hwvolt.h"

#include "target_functions.h"

// Add your target functions here
// TARGET(<name>, <prepare_function>)
// target function need to be declared in target_functions.h
// as "void *<name>_target(void* input);"
target_function_t targets[] = {
		TARGET(mul,     prefunc_randop12),                  // ID 0
		TARGET(cmp,     prefunc_randop12_same),             // ID 1
		TARGET(dummy,           prefunc_dummy),             // ID 2
};

int faulty_result_found=0;

int timeout = 10;
const int buf_max = 1024;
char eolchar = '\n';
int rc;
int delay_start=0;

void print_affinity() {
	cpu_set_t mask;
	long nproc, i;

	if (sched_getaffinity(0, sizeof(cpu_set_t), &mask) == -1) {
		perror("sched_getaffinity");
		assert(0);
	}
	nproc = sysconf(_SC_NPROCESSORS_ONLN);
	printf("sched_getaffinity = ");
	for (i = 0; i < nproc; i++) {
		printf("%d ", CPU_ISSET(i, &mask));
	}
	printf("\n");
}

//
void usage(void)
{
	printf("Usage: glitch_controller -b <bps> -p <serialport> [OPTIONS]\n"
			"\n"
			"Options:\n"
			"  -h, --help                 Print this help message\n"
			"  -b, --baud=baudrate        Baudrate (bps) of Arduino (default 115200)\n"
			"  -p, --port=teensySerial    Serial port Teensy is connected to\n"
			"  -s, --send=string          Send string to Arduino\n"
			"  -S, --sendline=string      Send string with newline to Arduino\n"
			"  -i  --stdinput             Use standard input\n"
			"  -r, --receive              Receive string from Arduino & print it out\n"
			"  -n  --num=num              Send a number as a single byte\n"
			"  -F  --flush                Flush serial port buffers for fresh reading\n"
			"  -e  --eolchar=char         Specify EOL char for reads (default '\\n')\n"
			"  -t  --timeout=millis       Timeout for reads in millisecs (default 5000)\n"
			"  -q  --quiet                Don't print out as much info\n"

			"  -g  --glitch               Do the voltage glitch\n"
            "  -d  --delay                Delay after trigger (Td) in uS"
			"      --iter		      	  Iterations of undervolting (assembly)\n"
			"      --num_p		          Number of svid packets (N)\n"
			"      --pre_volt	          Voltage before the glitch (Vp)\n"
			"      --pre_delay	          Delay before the glitch (TTp)\n"
			"      --glitch_voltage	      Glitch voltage (Vf)\n"
			"      --rst_volt	          Reset voltage (Vn)\n"
			"      --rst_delay	          Reset delay (TTf)\n"

			"      --target_id	          The target function id (check the targets h file) 0:MUL, 1:CMP\n"
            "      --targer_name          The name of the targer, e.g the name of \"mul_target\" is \"mul\""
			"      --calc_op1	          Operand 1\n"
			"      --calc_op2	          Operand 2\n"
			"      --calc_max_or_fixed_op1\n"
			"      --calc_max_or_fixed_op2\n"
			"      --calc_op1_min\n"
			"      --calc_op2_min\n"
			"      --calc_thread_num	  Numbers of thread\n"
			"      --retries 			  Retries of every configuration\n"
			"\n"
			"Note: Order is important. Set '-b' baudrate before opening port'-p'. \n"
			"      Used to make series of actions: '-d 2000 -s hello -d 100 -r' \n"
			"      means 'wait 2secs, send 'hello', wait 100msec, get reply'\n"
			"\n");
	exit(EXIT_SUCCESS);
}

//
void error(char* msg)
{
	fprintf(stderr, "%s\n",msg);
	exit(EXIT_FAILURE);
}
void set_freq(char* freq){
	char cmd[200];
	sprintf(cmd, "cpupower -c all frequency-set -u %s", freq);
	system(cmd);
	sprintf(cmd, "cpupower -c all frequency-set -d %s", freq);
	system(cmd);

	sleep(2);
	system("grep \"cpu MHz\" /proc/cpuinfo");
}
static int command_flag;
// Options
#define OPTION_ITERATIONS 1
#define OPTION_PACKET_NUM 2
#define OPTION_PRE_VOLTAGE 3
#define OPTION_PRE_DELAY 4
#define OPTION_GLITCH_VOLTAGE 5
#define OPTION_RST_VOLTAGE 6
#define OPTION_RST_DELAY 7
#define OPTION_TARGET_ID 8
#define OPTION_CALC_MAX_OR_FIXED_OP1 9
#define OPTION_CALC_MAX_OR_FIXED_OP2 10
#define OPTION_CALC_OP1 11
#define OPTION_CALC_OP2 12
#define OPTION_CALC_OP1_MIN 13
#define OPTION_CALC_OP2_MIN 14
#define OPTION_CALC_THREAD_NUM 15
#define OPTION_RETRIES 16
#define OPTION_FREQUENCY 17
#define OPTION_SIMPLE 18
#define OPTION_START_V 19
#define OPTION_STEP_COUNT 20
#define OPTION_TARGET_NAME 21


// Assign this process to certain core (For benchmark)
// #define SET_AFFINITY

int main(int argc, char *argv[])
{
	// Show temperature
	system("sensors");

#ifdef SET_AFFINITY
	// assign the process to be running on core 1
	cpu_set_t my_set;        // Define your cpu_set bit mask.
	CPU_ZERO(&my_set);       // Initialize it all to 0, i.e. no CPUs selected.
	CPU_SET(1, &my_set);     // set the bit that represents core 1.
	sched_setaffinity(0, sizeof(cpu_set_t), &my_set); // Set affinity of tihs process to
	print_affinity();       // CPU core mask, indicating which core this process will be running on.
#endif

	// Init func id
	int targets_list_len = sizeof(targets)/sizeof(target_function_t);
	for (int i=0; i< targets_list_len; i++){
		targets[i].func_id = i;
	}

	char serialport[buf_max];
	char buf[buf_max];
	int n;
	int baudrate = 115200;  // default
	char quiet=0;
	long retries;

	if (argc==1) {
		usage();
	}

	/* parse options */
	int option_index = 0, opt;
	static struct option loptions[] = {
		{"help",       no_argument,       0, 'h'},
		{"port",       required_argument, 0, 'p'},
		{"baud",       required_argument, 0, 'b'},
		{"send",       required_argument, 0, 's'},
		{"sendline",   required_argument, 0, 'S'},
		{"stdinput",   no_argument,       0, 'i'},
		{"receive",    no_argument,       0, 'r'},
		{"flush",      no_argument,       0, 'F'},
		{"num",        required_argument, 0, 'n'},
		{"eolchar",    required_argument, 0, 'e'},
		{"timeout",    required_argument, 0, 't'},
		{"quiet",      no_argument,       0, 'q'},
		{"glitch",      no_argument,       0, 'g'},

		{"iter",      required_argument, &command_flag , OPTION_ITERATIONS},
		{"num_p",      required_argument, &command_flag , OPTION_PACKET_NUM},
		{"delay",      required_argument, 0, 'd'},
		{"pre_volt",      required_argument, &command_flag , OPTION_PRE_VOLTAGE },
		{"pre_delay",      required_argument, &command_flag, OPTION_PRE_DELAY },
		{"glitch_voltage",      required_argument, &command_flag, OPTION_GLITCH_VOLTAGE },
		{"rst_volt",      required_argument, &command_flag, OPTION_RST_VOLTAGE },
		// Rst delay can be negative number, as teensy4 is will calculate and add some delay using the slew rate
		{"rst_delay",      required_argument, &command_flag, OPTION_RST_DELAY },

		{"target_id",      required_argument, &command_flag, OPTION_TARGET_ID },
		{"target_name",      required_argument, &command_flag, OPTION_TARGET_NAME },
		{"calc_max_or_fixed_op1",      required_argument, &command_flag, OPTION_CALC_MAX_OR_FIXED_OP1 },
		{"calc_max_or_fixed_op2",      required_argument, &command_flag, OPTION_CALC_MAX_OR_FIXED_OP2 },
		{"calc_op1",      required_argument, &command_flag, OPTION_CALC_OP1 },
		{"calc_op2",      required_argument, &command_flag, OPTION_CALC_OP2 },
		{"calc_op1_min",      required_argument, &command_flag, OPTION_CALC_OP1_MIN },
		{"calc_op2_min",      required_argument, &command_flag, OPTION_CALC_OP2_MIN },
		{"calc_thread_num",      required_argument, &command_flag, OPTION_CALC_THREAD_NUM },    // To be implemented
		{"retries",      required_argument, &command_flag, OPTION_RETRIES },
		{"freq",      required_argument, &command_flag, OPTION_FREQUENCY },

		{"simple",      no_argument, &command_flag, OPTION_SIMPLE }, // simple mode
		{"start_voltage",      required_argument, &command_flag, OPTION_START_V }, // simple mode - start voltage
		{"step_count",      required_argument, &command_flag, OPTION_STEP_COUNT }, // simple mode - count
		{NULL,         0,                 0, 0}
	};


	calculation_info* calc_info = malloc(sizeof(calculation_info));
	if ( !calc_info ){
		exit(EXIT_FAILURE);
	}

	undervoltage_info* undr_info =  malloc(sizeof(undervoltage_info));
	if ( !undr_info ){
		exit(EXIT_FAILURE);
	}
	undr_info->target_func_id = -1;

	int params_present = 0;
	int flag_simple_mode = 0;
	char * target_name = 0;
	while(1) {
		opt = getopt_long (argc, argv, "hp:b:s:S:i:rFn:d:qe:t:gc",
				loptions, &option_index);
		if (opt==-1) break;
		switch (opt) {
			case 0:
				switch(command_flag){
					case OPTION_ITERATIONS:
						calc_info->max_iterations = strtoll(optarg,NULL,10);
						printf("max_iterations %lu\n",calc_info->max_iterations);
						break;
					case OPTION_RETRIES:
						retries = strtol(optarg,NULL,10);
						printf("retries %li\n", retries );
						break;
					case OPTION_PACKET_NUM:
						undr_info->num_packets= atoi(optarg);
						printf("num_packets %d\n", undr_info->num_packets);
						break;
					case OPTION_PRE_VOLTAGE:
						undr_info->pre_voltage =  atof(optarg);
						printf("pre_voltage %f\n", undr_info->pre_voltage);
						break;
					case OPTION_PRE_DELAY:
						undr_info->pre_delay = atoi(optarg);
						printf("pre_delay  %d\n", undr_info->pre_delay );
						break;
					case OPTION_GLITCH_VOLTAGE:
						undr_info->glitch_voltage =  atof(optarg);
						printf("glitch_voltage %f\n", undr_info->glitch_voltage);
						break;
					case OPTION_RST_VOLTAGE:
						undr_info->rst_voltage =  atof(optarg);
						printf("rst_voltage %f\n", undr_info->rst_voltage );
						break;
					case OPTION_RST_DELAY:
						undr_info->rst_delay = atoi(optarg);
						printf("rst_delay %d\n", undr_info->rst_delay );
						break;
					case OPTION_TARGET_ID:
						undr_info->target_func_id = atoi(optarg);
						printf("target_func_id %d\n", undr_info->target_func_id);
						break;
					case OPTION_TARGET_NAME:
						target_name = optarg;

						for (int name_i =0; name_i<targets_list_len; name_i ++){
							if (strcmp(target_name, targets[name_i].name) == 0){
								undr_info->target_func_id = name_i;
							}
						}
						if (undr_info->target_func_id == -1){
							printf("Can not find target\n");
							return -1;
						}
						printf("Use %s target\n", target_name);
						printf("target_func_id %d\n", undr_info->target_func_id);
						break;
					case OPTION_CALC_MAX_OR_FIXED_OP1:
						calc_info->max_or_fixed_op1 = optarg[0];
						printf("max_or_fixed_op1 %c\n", calc_info->max_or_fixed_op1);
						break;
					case OPTION_CALC_MAX_OR_FIXED_OP2:
						calc_info->max_or_fixed_op2 = optarg[0];
						printf("max_or_fixed_op2 %c\n", calc_info->max_or_fixed_op2);
						break;
					case OPTION_CALC_OP1:
						calc_info->operand1 = strtoll(optarg,NULL, 16);
						printf("operand1 %lu\n", calc_info->operand1);
						params_present = 1;
						break;
					case OPTION_CALC_OP2:
						calc_info->operand2 = strtoll(optarg,NULL, 16);
						printf("operand2 %lu\n", calc_info->operand2);
						params_present = 1;
						break;
					case OPTION_CALC_OP1_MIN:
						calc_info->operand1_min = strtoll(optarg,NULL, 16);
						printf("operand1_min  %lu\n", calc_info->operand1_min);
						break;
					case OPTION_CALC_OP2_MIN:
						calc_info->operand2_min = strtoll(optarg,NULL, 16);
						printf("operand2_min  %lu\n", calc_info->operand2_min);
						break;
					case OPTION_FREQUENCY:
						printf("frequency %s\n", optarg);
						set_freq(optarg);
						sleep(1);
						break;
					case OPTION_CALC_THREAD_NUM:
						calc_info->thread_number = atoi(optarg);
						printf("thread %d\n", calc_info->thread_number);
						break;

					case OPTION_SIMPLE:
						printf("Simple Mode\n");
						flag_simple_mode = 1;
						break;
					case OPTION_START_V:
						undr_info->s_start_v = atof(optarg);
						printf("Start voltage: %lf", undr_info->s_start_v);
						break;
					case OPTION_STEP_COUNT:
						undr_info->s_step_count = atoi(optarg);
						printf("Step Count: %d", undr_info->s_step_count);
						break;
				}
				break;
			case 'g':
				// will make RS232 respond faster
				TRIGGER_RST
				TRIGGER_SET
				TRIGGER_RST

				printf("TEST:%d",undr_info->rst_delay);


				void *(*target_func)(void*);

				// Choose target function
				target_func = targets[undr_info->target_func_id].func;

				printf("------Start glitching------\n");

				if(flag_simple_mode) {
					for (int isc = 0; isc < undr_info->s_step_count; isc ++){
						for (int repeat = 0; repeat < retries; repeat++ ) {
							if (delay_start > 0) {
								configure_glitch_with_delay(1, undr_info->pre_voltage, \
                            35, undr_info->s_start_v - 0.005 * isc, -25, \
                            undr_info->pre_voltage, delay_start);
							} else {
								configure_glitch_with_delay(1, undr_info->pre_voltage, \
                            35, undr_info->s_start_v - 0.005 * isc, -25, \
                            undr_info->pre_voltage, 0);
							}
							if (!params_present) {
								//Initiates the operants with randon(?!) values
							targets[undr_info->target_func_id].prefunc(calc_info);
							}
							msleep(300);
							target_func(calc_info);

							if (faulty_result_found == 1) {
								//faulty_result_found=0;
								printf("Fault at step: %d\n", isc);
								break;
							}
							msleep(300);
							printf("Repeat: %d\n", repeat);
						}
						sleep(5);
						if (faulty_result_found == 1){
							break;
						}

					}
				}else {
				long c_retry;
				// Retry (N)
				for(c_retry=0; c_retry<retries; c_retry++){

					if ( delay_start > 0 ){
						configure_glitch_with_delay(undr_info->num_packets, undr_info->pre_voltage, \
							undr_info->pre_delay , undr_info->glitch_voltage, undr_info->rst_delay, \
							undr_info->rst_voltage, delay_start);
					}else{
						configure_glitch_with_delay(undr_info->num_packets, undr_info->pre_voltage, \
							undr_info->pre_delay , undr_info->glitch_voltage, undr_info->rst_delay, \
							undr_info->rst_voltage, 0);
					}
					if (! params_present ){
						//Initiates the operants with randon(?!) values
							targets[undr_info->target_func_id].prefunc(calc_info);
					}
					msleep(300);
					target_func(calc_info);

					if(faulty_result_found == 1){
						//faulty_result_found=0;
						printf("Fault at retry: %ld\n",c_retry);
						break;
					}
					msleep(300);
					//hwvolt_arm();
					}
				}
				break;
			case 'q':
				  quiet = 1;
				  break;
			case 'e':
				  eolchar = optarg[0];
				  if(!quiet) printf("eolchar set to '%c'\n",eolchar);
				  break;
			case 't':
				  timeout = strtol(optarg,NULL,10);
				  if( !quiet ) printf("timeout set to %d millisecs\n",timeout);
				  break;
			case 'd':
				  delay_start = strtol(optarg,NULL,10);
				  break;
			case 'h':
				  usage();
				  break;
			case 'b':
				  baudrate = strtol(optarg,NULL,10);
				  break;
			case 'p':
				  strcpy(serialport,optarg);
				  if( init_hw_volt("/dev/ttyS0",serialport,baudrate) == -1){
				  	return -1;
				  }
				  break;
			case 'n':
				  if( fd_teensy == -1 ) error("serial port not opened");
				  n = strtol(optarg, NULL, 10); // convert string to number
				  rc = serialport_writebyte(fd_teensy, (uint8_t)n);
				  if(rc==-1) error("error writing");
				  break;
			case 'S':
			case 's':
				  if( fd_teensy == -1 ) error("serial port not opened");
				  sprintf(buf, (opt=='S' ? "%s\n" : "%s"), optarg);

				  if( !quiet ) printf("send string:%s\n", buf);
				  rc = serialport_write(fd_teensy, buf);
				  if(rc==-1) error("error writing");
				  break;
			case 'i':
				  rc=-1;
				  if( fd_teensy == -1) error("serial port not opened");
				  while(fgets(buf, buf_max, stdin)) {
					  if( !quiet ) printf("send string:%s\n", buf);
					  rc = serialport_write(fd_teensy, buf);
				  }
				  if(rc==-1) error("error writing");
				  break;
			case 'r':
				  if( fd_teensy == -1 ) error("serial port not opened");
				  memset(buf,0,buf_max);  //
				  serialport_read_until(fd_teensy, buf, eolchar, buf_max, timeout);
				  if( !quiet ) printf("read string:");
				  printf("%s\n", buf);
				  break;
			case 'F':
				  if( fd_teensy == -1 ) error("serial port not opened");
				  if( !quiet ) printf("flushing receive buffer\n");
				  serialport_flush(fd_teensy);
				  break;

		}
	}

	destroy_hw_volt();

	free(undr_info);
	free(calc_info);
	exit(EXIT_SUCCESS);
} // end main

