//
// Created by Zitai Chen on 15/08/2020.
//

#define _GNU_SOURCE

#include "jitter_test.h"
#include "hwvolt.h"
#include "stdio.h"
#include <time.h>

#include <sched.h>    // for CPU schedulling
#include <assert.h>

#define ndelay(x)    udelay(((x)+999)/1000)

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

int main() {

	// assign the process to be running on core 1
	cpu_set_t my_set;        // Define your cpu_set bit mask.
	CPU_ZERO(&my_set);       // Initialize it all to 0, i.e. no CPUs selected.
	CPU_SET(1, &my_set);     // set the bit that represents core 1.
	sched_setaffinity(0, sizeof(cpu_set_t), &my_set); // Set affinity of tihs process to
	print_affinity();       // CPU core mask, indicating which core this process will be running on.

	// init hw volt
	if (init_hw_volt("/dev/ttyS0", "/dev/ttyACM0", 115200) == -1) {
		return -1;
	}

#ifdef TRIGGER_DTR
	// will make it respond faster
	TRIGGER_RST
	TRIGGER_SET
	TRIGGER_RST
#endif

	//configure_glitch_with_delay(1, 1.0, 10, 1.2, 10, 1.0, 0);
	hwvolt_arm();
	sleep(1);
	printf("Arm cmd sent\n");

	// Configure timespec
#ifdef SLEEP_TSC
	struct timespec tim, tim2;
   tim.tv_sec = 0;
   tim.tv_nsec = 200000;
#endif

   // Measure the interval between triggers and determine the jitter
	int src = 1000000;
	while (1) {
#ifdef SLEEP_NOP
				asm volatile
				(
					"movl  %0, %%esi \n\t"
					".delay2: \n\t"
					"dec %%esi \n\t"
					"jnz .delay2 \n\t"
					:
					: "r" (src)
				);
#endif
#ifdef SLEEP_TSC
		nanosleep(&tim, &tim2);
		//nanosleep(100);
#endif
		TRIGGER_SET
		//TRIGGER_RST
#ifdef SLEEP_NOP

				asm volatile
				(
					"movl  %0, %%esi \n\t"
					".delay1: \n\t"
					"dec %%esi \n\t"
					"jnz .delay1 \n\t"
					:
					: "r" (src)
				);
#endif
#ifdef SLEEP_TSC
		nanosleep(&tim, &tim2);
		//nanosleep(100);
#endif
	TRIGGER_RST


	}
}