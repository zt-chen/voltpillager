//
// Created by Zitai Chen on 27/04/2020.
// Need: sudo modprobe msr
// Call init_plundervolt first to initialize fd_msr
// WARMING: other functions will not check if fd_msr is opened or not, use them with cautious
//

#ifndef HW_CPU_UNDERVOLTING_PLUNDERVOLT_H
#define HW_CPU_UNDERVOLTING_PLUNDERVOLT_H
#include <fcntl.h>
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
// Initialize plundervolt (software undervolting)
int init_plundervolt();

// Generate msr config for undervolting voltage by X mV on plane Y
uint64_t wrmsr_value(int64_t val,uint64_t plane);

// Write to msr
// e.g. voltage_change(wrmsr_value(-200,2));
void voltage_change(uint64_t val);

// Reset the voltage to normal
void reset_voltage();

// Clean up and free memory
void destroy_plundervolt();

extern int fd_msr;
#endif //HW_CPU_UNDERVOLTING_PLUNDERVOLT_H
