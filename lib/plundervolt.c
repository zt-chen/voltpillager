//
// Created by Zitai Chen on 27/04/2020.
//
#include "plundervolt.h"

uint64_t plane0_zero, plane2_zero;
int fd_msr;

int init_plundervolt(){
	// Zero under-volt
	plane0_zero = wrmsr_value(0,0);
	plane2_zero = wrmsr_value(0,2);
	fd_msr = open("/dev/cpu/0/msr", O_WRONLY);
	if(fd_msr == -1)
	{
		printf("Could not open /dev/cpu/0/msr\n");
		return -1;
	}
	return 0;
}
uint64_t wrmsr_value(int64_t val,uint64_t plane)
{
	// -0.5 to deal with rounding issues
	val=(val*1.024)-0.5;
	val=0xFFE00000&((val&0xFFF)<<21);
	val=val|0x8000001100000000;
	val=val|(plane<<40);
	return (uint64_t)val;
}

void voltage_change(uint64_t val)
{
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	pwrite(fd_msr,&val,sizeof(val),0x150);
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
}

void reset_voltage()
{
	////////////////////
	// return voltage //
	////////////////////
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	voltage_change(plane0_zero);
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	voltage_change(plane2_zero);
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
	__asm__("nop");
#ifdef DEBUG
	printf("\r\nVoltage reset\r\n");
#endif
}
void destroy_plundervolt(){
	close(fd_msr);
}
