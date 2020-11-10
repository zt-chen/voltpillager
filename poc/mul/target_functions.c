//#include <immintrin.h>
//#include <x86intrin.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include "target_functions.h"
#include <termios.h>     /* POSIX Terminal Control Definitions */
#include <sys/ioctl.h>   /* ioctl()                            */
#include <termios.h>     /* POSIX Terminal Control Definitions */
#include <errno.h>       /* ERROR Number Definitions           */
#include <fcntl.h>       /* File Control Definitions           */
#include <emmintrin.h>
#include <time.h>
#define MY_ASM "rol    eax,0x3f;"

#include "hwvolt.h"
extern int faulty_result_found;


void prefunc_randop12(calculation_info* calc_info){
	time_t t;
	srand((unsigned) time(&t));
			calc_info->operand1 = ((uint64_t)rand()<<32) | (rand());
			calc_info->operand2 = ((uint64_t)rand()<<32) | (rand());
}
void prefunc_randop12_same(calculation_info* calc_info){
	time_t t;
	srand((unsigned) time(&t));
			calc_info->operand1 = ((uint64_t)rand()<<32) | (rand());
			calc_info->operand2 = calc_info->operand1;

}

void prefunc_randop1(calculation_info* calc_info){
	time_t t;
	srand((unsigned) time(&t));

	calc_info->operand1 = ((uint64_t)rand()<<32) | (rand());
}

void prefunc_dummy(calculation_info* calc_info){

}
void *mul_target(void* input)
{
	printf("MUL\n");
	calculation_info *ci=(calculation_info*)input;
	uint64_t iterations=ci->max_iterations;
	uint64_t max1=ci->operand1;
	uint64_t max2=ci->operand2;
	uint64_t min1=ci->operand1_min;
	uint64_t min2=ci->operand2_min;
	unsigned int one,two;

	ci->iterations_performed=0;
	if (ci->max_or_fixed_op1=='M')
	{
		__builtin_ia32_rdrand32_step(&one);
		ci->operand1=one % (max1 - min1) + min1;
	}
	if (ci->max_or_fixed_op2=='M')
	{
		__builtin_ia32_rdrand32_step(&two);
		ci->operand2=(two % (max2-min2)) + 1+min2 ;
	}

	// Trigger
	int performed = ci->iterations_performed;
	TRIGGER_SET
	do
	{
		performed++;
		ci->correct_a = ci->operand1 * ci->operand2;
		ci->correct_b = ci->operand1 * ci->operand2;
		if(ci->correct_a!=ci->correct_b){
			faulty_result_found = 1;
		}
	} while ( faulty_result_found == 0 && performed<iterations);
	// Reset Trigger
	TRIGGER_RST


	if (faulty_result_found==1)
	{
		//faulty_result_found=1;
		ci->iterations_performed = performed ;
		printf("\n------   [MUL] CALCULATION ERROR DETECTED   ------\n");
		printf(" > Iterations  \t : %08li\n"  ,ci->iterations_performed);
		printf(" > Operand 1   \t : %016lx\n" ,ci->operand1);
		printf(" > Operand 2   \t : %016lx\n" ,ci->operand2);
		printf(" > Correct     \t : %016lx\n" ,ci->operand1*ci->operand2);
		if (correct!=ci->correct_a)
			printf( " > Result      \t : %016lx\n" , ci->correct_a);
		if (correct!=ci->correct_b)
			printf( " > Result      \t : %016lx\n" , ci->correct_b);
		printf( " > xor result  \t : %016lx\n" , ci->correct_a^ci->correct_b);
		fflush(stdout);

		// Get temperature
		system("sensors");
		//printf( " > undervoltage\t : %i\n"     , undervoltage_data.voltage+1);


	}
	return NULL;
}
void *cmp_target(void* input){
	printf("CMP\n");

	calculation_info *ci=(calculation_info*)input;
	uint64_t iterations=ci->max_iterations;

	int i = 0;
	TRIGGER_SET
	// Type your code here, or load an example.

	uint64_t operand1 = ci->operand1;
	uint64_t operand2 = ci->operand2;

	do{
		if(operand1 != operand2){
			faulty_result_found = 1;
		}
		operand1 ++;
		operand2 ++;
		i++;
	}while(faulty_result_found==0 && i<iterations);
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");

	if(faulty_result_found == 0){
		TRIGGER_RST
	}
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
	ci->iterations_performed = i;

	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
    if (faulty_result_found==1)
    {
        printf("\n------   CALCULATION ERROR DETECTED   ------\n");
	    printf(" > OpCode CMP (if)\n" );
        printf(" > Iterations  \t : %08li \n"  ,ci->iterations_performed);
        printf(" > initial operands1: %016li \n", ci->operand1);
	    printf(" > initial operands2: %016li \n", ci->operand2);
	    printf(" > Operand 1   \t : %016li \n" ,operand1);
        printf(" > Operand 2   \t : %016li \n" ,operand2);
	    printf(" > Result    \t : %id\n" ,faulty_result_found);
        //printf( " > undervoltage\t : %i\n"     , undervoltage_data.voltage+1);
	    fflush(stdout);
	    // Get temperature
	    system("sensors");

    }

	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");
	__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");__asm__("nop");

	return NULL;
}

void *dummy_target(void* input){
	TRIGGER_SET
	msleep(5);
	TRIGGER_RST

	return NULL;
}