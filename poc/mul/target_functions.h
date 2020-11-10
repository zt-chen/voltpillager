#ifndef TARGET_FUNCTIONS_H
#define TARGET_FUNCTIONS_H
#include "hwvolt.h"
uint64_t result_eax, result_xmm0, iterations_performed,correct;
typedef struct calculation_info_t
{
        char max_or_fixed_op1;
        char max_or_fixed_op2;
        uint64_t operand1;
        uint64_t operand2;
        uint64_t operand1_min;
        uint64_t operand2_min;
        uint64_t correct_a;
        uint64_t correct_b;
        uint64_t iterations_performed;
        int thread_number;
	int thread_id;
	uint64_t max_iterations;
} calculation_info;

typedef struct undervoltage_info_t
{
	int  num_packets;
	double  pre_voltage;
	int 	pre_delay;
	double	glitch_voltage;
	double	rst_voltage;
	int 	rst_delay;
	int 	target_func_id;

	double  s_start_v;
	int     s_step_count;
} undervoltage_info;


void prepareCalculation(int func_id, calculation_info* calc_info);

// Add your target function declaration at here
void *mul_target(void* input);
void *cmp_target(void* input);
void *dummy_target(void* input);

void prefunc_randop12(calculation_info* calc_info);
void prefunc_randop12_same(calculation_info* calc_info);
void prefunc_randop1(calculation_info* calc_info);
void prefunc_dummy(calculation_info* calc_info);

typedef struct target_function {
	int func_id;
	const char* name;
	void *(*func)(void*);
	void (*prefunc)(calculation_info* calc_info);
} target_function_t;
#define TARGET(NAME, PREFUNC)  {-1, #NAME, NAME ## _target, PREFUNC }
#endif
