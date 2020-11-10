//
// Created by Zitai Chen on 10/05/2020.
//

#ifndef HW_CPU_UNDERVOLTING_HWVOLT_H
#define HW_CPU_UNDERVOLTING_HWVOLT_H

#include <sys/ioctl.h>   // ioctl()
#include <termios.h>	 // POSIX Terminal Control Definitions
#include <unistd.h>      // for usleep()

#include "arduino-serial-lib.h"
#define BUFMAX 1024
#define EOLCHAR '\n'
#define TIMEOUT 10

#define NOP __asm__("nop")
#define ten(a)     a;a;a;a;a;a;a;a;a;a
#define fifty(a)   ten(a);ten(a);ten(a);ten(a);ten(a)
#define handred(a) ten(ten(a))
#ifdef __cplusplus
extern "C"{
#endif
// initialize serial ports
int init_hw_volt(char* const trigger_serial, char* const teensy_serial, int teensy_baudrate);

// send configuration commands to teensy
// Internal use only, or need to call arm after using this
int configure_glitch(int repeat, float v1, int d1, float v2, int d2, float v3);

// Configure undervolting parameters and arm the glitch
int configure_glitch_with_delay(int repeat, float v1, int d1, float v2, int d2, float v3, int delay_before_glitch_us);

// Cleanup and free memory
int destroy_hw_volt();

// Trigger using Teensy USB
// Internal use only, use TRIGGER_TEENSY marco and call TRIGGER_SET instead
int teensy_serial_trigger();

// Arm the glitch
int hwvolt_arm();
#ifdef __cplusplus
}
#endif

///////////////////////global variables////////////////////////
extern int fd_teensy;    // fd for Teensy
extern int fd_trigger;   // fd for onboard COM(RS232) port

extern int RTS_flag;
extern int DTR_flag;
///////////////////////inline functions////////////////////////
// Use the DTR signal of another serial as trigger
#ifdef TRIGGER_DTR
#define TRIGGER_SET     ioctl(fd_trigger,TIOCMBIS,&DTR_flag);    // set -> LOW
#define TRIGGER_RST     ioctl(fd_trigger,TIOCMBIC,&DTR_flag);    // clear -> HIGH
#define TRIGGER_PULSE   ioctl(fd_trigger,TIOCMBIS,&DTR_flag);  ioctl(fd_trigger,TIOCMBIC,&DTR_flag); // ----__----

// Use teensy serial(USB) as trigger
#else
#ifdef TRIGGER_TEENSY
#define TRIGGER_SET      teensy_serial_trigger();
#define TRIGGER_RST      ;
#define TRIGGER_PULSE    ;
#endif
#endif

#define msleep(tms) ({usleep(tms * 1000);})

// Clock tick measurement
#if defined(__i386__)
static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}
#elif defined(__x86_64__)
static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}
#endif

#endif //HW_CPU_UNDERVOLTING_HWVOLT_H
