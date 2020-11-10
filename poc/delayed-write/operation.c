#define _GNU_SOURCE
#include <fcntl.h>
#include <curses.h>
#include <immintrin.h>
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>

int verbose=1, display_calculation=0, run_compare=0;
int fd;
int faulty_result_found=0;
int undervolting_finished=0;
uint64_t plane0_zero;
uint64_t plane2_zero;
int pp_y=6;
int pp_x=1;
char printme[100];
WINDOW *mainwin;


typedef struct calcuation_info_t
{
	char max_or_fixed_op1;
	char max_or_fixed_op2;
	uint64_t starting_operand1;
	uint64_t operand1;
	uint64_t operand2;
	uint64_t operand1_min;
	uint64_t operand2_min;
	uint64_t correct_a;
	uint64_t correct_b;
	uint64_t iterations_performed;
	int thread_number;
} calculation_info;

typedef struct undervoltage_info_t
{
	int start_undervoltage;
	int end_undervoltage;
	int stop_after_x_drops;
	int voltage_steps;
	int voltage;
	float time_spent;
	uint64_t milli_secs;;
} undervoltage_info;


uint64_t wrmsr_value(int64_t val,uint64_t plane)
{
	// -0.5 to deal with rounding issues
	val=(val*1.024)-0.5;
	val=0xFFE00000&((val&0xFFF)<<21);
	val=val|0x8000001100000000;
	val=val|(plane<<40);
	return (uint64_t)val;
}

void voltage_change(int fd, uint64_t val)
{
	pwrite(fd,&val,sizeof(val),0x150);
}

void reset_voltage()
{
	////////////////////
	// return voltage //
	////////////////////
	voltage_change(fd,plane0_zero);
	voltage_change(fd,plane2_zero);
	if (verbose)
		printf("\nVoltage reset\n");
}

void *undervolt_it(void *input)
{
	int s ;
	cpu_set_t cpuset;
	pthread_t thread;
	thread = pthread_self();
	 
	CPU_ZERO(&cpuset);
	/* Set affinity mask to CPUs 0  */
	CPU_SET(0, &cpuset);

	s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
	if (s != 0) 
  	{
		printf("pthread set affinity error\r\n");
	}
	 
	undervoltage_info *ui=(undervoltage_info*)input;
	ui->voltage=ui->start_undervoltage;
	uint64_t milli_secs=ui->milli_secs * CLOCKS_PER_SEC / 1000; 
	int counter=0;
	clock_t start_time, end_time;
	while ( faulty_result_found==0 
	&& ( ui->voltage > ui->end_undervoltage ||  ui->stop_after_x_drops>counter))
	{
		if (verbose)
		{
			printf("\r\nUndervolting: %i\r\n",ui->voltage);;
		}
		counter++;
		
		voltage_change(fd,wrmsr_value(ui->voltage,0));
		voltage_change(fd,wrmsr_value(ui->voltage,2));
		start_time =clock();
		while ((end_time< start_time+milli_secs) && faulty_result_found==0)
		{
			end_time=clock();
		}
		ui->voltage=ui->voltage - ui->voltage_steps;
		ui->time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	}

	reset_voltage(); 
	/* calculate time spent */
	ui->time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	undervolting_finished=1;
	return NULL;
}

void screen_init()
{
	if ((mainwin=initscr()) == NULL)
	{
		fprintf(stderr, "Error initialising ncurses.\n");
		exit (EXIT_FAILURE);
	}	
	start_color();
	init_pair(1, COLOR_GREEN,  COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_pair(3, COLOR_BLACK, COLOR_RED);
	init_pair(4, COLOR_WHITE, COLOR_RED);
	color_set(1,NULL);
}

void screen_del()
{
	delwin(mainwin);
	endwin();
	refresh();
}

void print_calculation(uint64_t one, uint64_t two, int thread_number)
{
	curs_set(0);
	color_set(2,NULL);
	sprintf(printme, " ---- core %i ---- ",thread_number);
	mvaddstr( 2 + (thread_number*3),pp_x+10,printme);
	color_set(1,NULL);
	sprintf(printme, "0x%08lx * 0x%08lx = 0x%016lx    ",one, two, one*two);
	mvaddstr( 3 + (thread_number*3),pp_x,printme);
	refresh();
}

// This is the actual size for our test
#define ARRAY_SIZE_MAX 16

// This is the part of the array that would be the whole array in reality
#define ARRAY_SIZE_TEST 8

// Some data to write into the array
#define NORMAL_VAL 0x1212121212121212
#define NORMAL_VAL_BYTE 0x12
#define OVERWRITE_VAL 0xABABABABABABABAB

void *compare_it(void *input)
{
	calculation_info *ci=(calculation_info*)input;
	uint64_t iterations=ci->iterations_performed;
    uint64_t i;
    volatile int my_faulty_result=0;
	
    int64_t copy_size = 0;
	uint64_t array[ARRAY_SIZE_MAX]={0};

	while (faulty_result_found==0 && undervolting_finished==0)
	{
		memset((void*)array, NORMAL_VAL_BYTE, ARRAY_SIZE_MAX * sizeof(uint64_t));
		
		i=0;

		do
		{
		    copy_size = ARRAY_SIZE_TEST + 4;
			
			// Hard limit the size here...
			if(copy_size >= ARRAY_SIZE_TEST)
				copy_size = ARRAY_SIZE_TEST - 1;
 
			// Overwrite elements 7 ... 1 (in normal case)
			while(copy_size >= 1)
			{
				array[copy_size] = OVERWRITE_VAL;
				copy_size--;
			}
			
			// Check if some element that should not have changed have changed
			if (array[ARRAY_SIZE_TEST] != NORMAL_VAL || array[0] != NORMAL_VAL)
			{
				my_faulty_result = 1;
			}
			
            i++;
		} while (my_faulty_result==0 && i<iterations);
	
		if (my_faulty_result==1)
		{
            faulty_result_found=1;  // This will tell the other threads to stop - including the undervolting thread
	   	}
	}


	if (my_faulty_result==1)
		{
			sleep(0.5);   // To give the undervolting thread a chance to reset the voltage
			// copy the faulty information
    		ci->correct_b=1;  // this is just a way to make correct_a != correct_b which is used for displaying the results later. (HACKY!!)
    		ci->iterations_performed=i;
		    ci->operand1=array[0];
		    ci->operand2=array[ARRAY_SIZE_TEST];
		    // Display the actual array
		    printf("\r\n");
		    for (int i=0;i<ARRAY_SIZE_MAX;i++)
		    {
		    	printf("[%02i] 0x%016lx \r\n",i,array[i]);
		    }
	   	}

	return NULL;
}


void *multiply_it(void *input)
{
	calculation_info *ci=(calculation_info*)input;
	uint64_t iterations=ci->iterations_performed;
	uint64_t max1=ci->operand1;
	uint64_t max2=ci->operand2;
	uint64_t min1=ci->operand1_min;
	uint64_t min2=ci->operand2_min;
	long long unsigned int one,two;
	

	while (faulty_result_found==0 && undervolting_finished==0)
	{
		ci->iterations_performed=0;
		if (ci->max_or_fixed_op1=='M')
		{
			__builtin_ia32_rdrand64_step(&one);
			ci->operand1=one % (max1 - min1) + min1;

		}
		if (ci->max_or_fixed_op2=='M')
		{
			__builtin_ia32_rdrand64_step(&two);
			ci->operand2=(two % (max2-min2)) + 1+min2 ;
		}
		/* SHOW THE CALCULATION ON THE SCREEN */
		if (display_calculation)
			print_calculation(ci->operand1,ci->operand2,ci->thread_number);
		do
		{
			ci->iterations_performed++;
			ci->correct_a = ci->operand1 * ci->operand2;
			ci->correct_b = ci->operand1 * ci->operand2;
		} while (ci->correct_a==ci->correct_b && 
                         ci->iterations_performed<iterations && 
			 faulty_result_found==0 &&
			 undervolting_finished==0);
	
		if (ci->correct_a!=ci->correct_b && faulty_result_found==0)
		{
			faulty_result_found=1;
		}
	}

	return NULL;
}


void usage(const char* program)
{
	printf("So many options - so little time:\n\n");
	printf("\t -m #           \t millisecs between voltage drop\n");
	printf("\t -i #           \t iterations\n");
	printf("\t -s #           \t start voltage\n");
	printf("\t -e #           \t end voltage\n");
	printf("\t -X #           \t Stop after x drops\n");
	printf("\t -1 0x#         \t operand1\n");
	printf("\t -2 0x#         \t operand2\n");
	printf("\t -t #           \t number of threads - default=1\n");
	printf("\t -v #           \t voltage_steps\n");
	printf("\t -z fixed | max \t fixed|max (what is operand 1 - default=fixed)\n");
	printf("\t -x fixed | max \t fixed|max (what is operand 2 - default=fixed)\n");
	printf("\t -q #           \t operand 1 minimum - default=0\n");
	printf("\t -w #           \t operand 2 minimum - default=0\n");
	printf("\t -S             \t Silent mode - default=verbose\n");
	printf("\t -M             \t Multiply only (no undervolting)\n");
	printf("\t -P             \t run compare (not multiplication)\n");
	printf("\t -C             \t display Calculation \n");
	printf("\t -h             \t display this Help\n");

}
int main (int argc, char* argv[])
{
	const char* program=argv[0];
	if (argc < 5 )
	{
		usage(program);
		return -1;
	}

	fd = open("/dev/cpu/0/msr", O_WRONLY);
	if(fd == -1)
	{
		printf("Could not open /dev/cpu/0/msr\n");
		return -1;
	}
	uint64_t iterations = 1000;
	uint64_t operand1=0, operand2=0;
	int64_t start_undervoltage=0,end_undervoltage=0;
	int stop_after_x_drops=0;
	int voltage_steps=1;
	char max_or_fixed_op1='F', max_or_fixed_op2='F';
	uint64_t operand1_min=0,operand2_min=0;
	uint64_t milli_secs=1000;
	int number_of_threads=1;
	int multiply_only=0;
	int opt=0;

    while((opt = getopt(argc, argv, "m:e:i:s:v:t:1:2:q:w:X:z:x:SMChP")) != -1)  
    {  
        switch(opt)  
        {  
	    case '1':
		operand1=strtol(optarg,NULL,16);
		break;
	    case '2':
		operand2=strtol(optarg,NULL,16);
		break;
            case 'z':
		if (strcmp(optarg,"fixed")==0)
			max_or_fixed_op1='F';
		else if (strcmp(optarg,"max")==0)
			max_or_fixed_op1='M';
		else
		{
			printf ("Error setting operand1 to be fixed or max value\n");
			return -1;
		}
                break;  
            case 'x':
		if (strcmp(optarg,"fixed")==0)
			max_or_fixed_op2='F';
		else if (strcmp(optarg,"max")==0)
			max_or_fixed_op2='M';
		else
		{
			printf ("Error setting operand2 to be fixed or max value\n");
			return -1;
		}
                break;  
	    case 'i':
	        iterations = strtol(optarg,NULL,10);
		break;
	    case 'h':
	        usage(program);
		return -1;
            case 's':  
		start_undervoltage=strtol(optarg,NULL,10);
                break;  
            case 'e':  
		end_undervoltage=strtol(optarg,NULL,10);
                break;  
            case 'q':  
		operand1_min=strtol(optarg,NULL,16);
                break;  
            case 'w':  
		operand2_min=strtol(optarg,NULL,16);
                break;  
            case 'v':  
		voltage_steps=strtol(optarg,NULL,10);
                break;  
            case 't':  
		number_of_threads=strtol(optarg,NULL,10);
                break;  
            case 'm':  
		milli_secs=strtol(optarg,NULL,10);
                break;  
            case 'X':  
		stop_after_x_drops=strtol(optarg,NULL,10);
                break;  
            case 'S':  
		verbose=0;
                break;  
	        case 'M':
		    multiply_only=1;
		break;
	    case 'C':
		display_calculation=1;
		break;
	    case 'P':
		run_compare=1;
		break;
            case ':':  
                printf("option needs a value\n");  
                break;  
            case '?':  
                printf("unknown option: %c\n", optopt); 
                break;  
        }  
    }  
    // optind is for the extra arguments 
    // which are not parsed 
    	for(; optind < argc; (optind++))
	{      
        	printf("extra arguments: %s\n", argv[optind]);  
		usage(program);
		return -1;
    	} 
	if (multiply_only==0 && start_undervoltage==0)
	{
		printf ("Error: start voltage must be set\n");
		return -1;
	}
	if (multiply_only==0 && end_undervoltage==0 && stop_after_x_drops==0)
	{
		printf ("Error: end voltage or end after x drops must be set.\n");
		return -1;
	}
	if (number_of_threads>0 && (operand1==0 || operand2==0) && run_compare==0)
	{
		printf ("Error: operand1 and operand2 must be set for multiplications.\n");
		return -1;
	}
	if (number_of_threads>0 && (operand1==0) && run_compare==1)
	{
		operand1=0xFFFFFFFFFF;
		max_or_fixed_op1='M';
	}
      
      
	if (verbose)
	{
		printf("\nSummary\n");
		printf("-------------------------------------------------\n");
		printf("time (ms) interval:      %li\n",milli_secs);
		printf("Iterations:         	 %li\n",iterations);
		printf("Start Voltage:      	 %li\n",start_undervoltage);
		printf("End Voltage:        	 %li\n",end_undervoltage);
		printf("Stop after x drops: 	 %i\n",stop_after_x_drops);
		printf("Voltage steps:      	 %i\n",voltage_steps);
		printf("Threads:            	 %i\n",number_of_threads);
		printf("Operand1:           	 0x%016lx\n",operand1);
		printf("Operand2:           	 0x%016lx\n",operand2);
		printf("Operand1 is:        	 %s\n",max_or_fixed_op1=='M'?"maximum":"fixed value");
		printf("Operand2 is:        	 %s\n",max_or_fixed_op2=='M'?"maximum":"fixed value");
		printf("Operand1 min is:    	 0x%016lx\n",operand1_min);
		printf("Operand2 min is:    	 0x%016lx\n",operand2_min);
		printf("Multiply only:	    	 %s\n",multiply_only==1?"Yes":"No");
		printf("Display calculation:	 %s\n",display_calculation==1?"Yes":"No");
		printf("Mode:	                 %s\n",run_compare==1?"COMPARE":"MULTIPLY");
	}
	plane0_zero = wrmsr_value(0,0);
	plane2_zero = wrmsr_value(0,2);

	pthread_t* my_calculation_thread=malloc(sizeof(pthread_t)*number_of_threads);
	calculation_info* calculation_data=malloc(sizeof(calculation_info)*number_of_threads);
	for (int i=0;i<number_of_threads;i++)
	{
		calculation_data[i].starting_operand1=operand1;
		calculation_data[i].operand1=operand1;
		calculation_data[i].operand2=operand2;
		calculation_data[i].operand1_min=operand1_min;
		calculation_data[i].operand2_min=operand2_min;
		calculation_data[i].max_or_fixed_op1=max_or_fixed_op1;
		calculation_data[i].max_or_fixed_op2=max_or_fixed_op2;
		calculation_data[i].correct_a=0;
		calculation_data[i].correct_b=0;
		calculation_data[i].iterations_performed=iterations;
		calculation_data[i].thread_number=i;
		if (run_compare==1)
		{
			pthread_create(&my_calculation_thread[i], NULL, compare_it, &calculation_data[i]);
		}
		else
		{
			pthread_create(&my_calculation_thread[i], NULL, multiply_it, &calculation_data[i]);
		}
	}
	
	undervoltage_info undervoltage_data;
	undervoltage_data.start_undervoltage=start_undervoltage;
	undervoltage_data.end_undervoltage=end_undervoltage;
	undervoltage_data.stop_after_x_drops=stop_after_x_drops;
	undervoltage_data.voltage_steps=voltage_steps;
	undervoltage_data.milli_secs=milli_secs;

	screen_init();



	if (multiply_only==0)
	{
		pthread_t my_undervolting_thread;
		pthread_create(&my_undervolting_thread,NULL,undervolt_it,&undervoltage_data);
		pthread_join(my_undervolting_thread, NULL);
	}

	for (int i=0;i<number_of_threads;i++)
	{
    		pthread_join(my_calculation_thread[i], NULL);
	}
    	

	for (int i=0;i<number_of_threads;i++)
	{
		if (calculation_data[i].correct_a!=calculation_data[i].correct_b)
		{
			clear();
			pp_y=3;
			uint64_t correct=calculation_data[i].operand1*calculation_data[i].operand2;
			color_set(4,NULL);
			mvaddstr(pp_y++,0,"------   CALCULATION ERROR DETECTED   ------");
			color_set(1,NULL);
			mvprintw(pp_y++,pp_x, " > Time spent     \t  : %f seconds\n" , undervoltage_data.time_spent); 
			mvprintw(pp_y++,pp_x, " > Max Iterations \t  : %08li\n"  , iterations);
			mvprintw(pp_y++,pp_x, " > Milli_secs     \t  : %08li\n"  , milli_secs);
			mvprintw(pp_y++,pp_x, " > Faulted Thread \t  : %i/%i\n"  , i,number_of_threads);
			mvprintw(pp_y++,pp_x, " > Iterations     \t  : %08li\n"  , calculation_data[i].iterations_performed);
            if (run_compare == 0)
            {
			    mvprintw(pp_y++,pp_x, " > Operand 1   \t  : %016lx\n" , calculation_data[i].operand1);
			    mvprintw(pp_y++,pp_x, " > Operand 2   \t  : %016lx\n" , calculation_data[i].operand2);
			    mvprintw(pp_y++,pp_x, " > Correct     \t  : %016lx\n" , calculation_data[i].operand1*calculation_data[i].operand2);
			    if (correct!=calculation_data[i].correct_a)
				    mvprintw(pp_y++,pp_x," > Result      \t  : %016lx\n" , calculation_data[i].correct_a);
			    if (correct!=calculation_data[i].correct_b)
				    mvprintw(pp_y++,pp_x," > Result      \t  : %016lx\n" , calculation_data[i].correct_b);
            }
            else
            {
			    mvprintw(pp_y++,pp_x, " > array[0]      \t  : %016lx\n" , calculation_data[i].operand1);
			    mvprintw(pp_y++,pp_x, " > array[ARRAY_SIZE_TEST]: %016lx\n" , calculation_data[i].operand2);
			    mvprintw(pp_y++,pp_x, " > Starting op1  \t  : %016lx\n"  ,calculation_data[i].starting_operand1 ); 

            }
			mvprintw(pp_y++,pp_x," > Mode           \t  : %s\n" , run_compare==1?"COMPARE":"MULTIPLY");
            mvprintw(pp_y++,pp_x," > xor result     \t  : %016lx\n" , calculation_data[i].correct_a^calculation_data[i].correct_b);
			mvprintw(pp_y++,pp_x," > undervoltage   \t  : %i\n"     , undervoltage_data.voltage+1);
			char* cmd1="sensors | grep \"Core 0\" |awk '{print $3}'i | tr -d '\\n'";
			FILE *fp=popen(cmd1,"r");
			char temp[10];
			fgets(temp,10,fp);
			fclose(fp);
			mvprintw(pp_y++,pp_x, " > temperature \t  : %s\n",temp);
			char* cmd2="cat /proc/cpuinfo | grep MHz | head -n 1 | cut  -d ':' -f2| cut -d '.' -f1 | tr -d '\\n'";
			fp=popen(cmd2,"r");
			char freq[10];
			fgets(freq,10,fp);
			mvprintw(pp_y++,pp_x, " > Frequency   \t  :%sMHz\n",freq);
			fclose(fp);	
			mvprintw(21,1," [Done] Press any key.");
			refresh();
			getch();
			screen_del();
		}
	}
    	delwin(mainwin);
   	endwin();
	printf("[Done]\n");
	return 0;
}
