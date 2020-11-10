# Undervolting Library
This is the library for controlling the CPU voltage, via software(plundervolt) or hardware(VoltPillager). 

All PoCs of this repo use this library.

## Folder Structure
```
├── arduino-serial              // Arduino Serial Library
├── hwvolt.c                    // VoltPillager Controlling Lib 
├── hwvolt.h                    // (Hardware Undervolting) 
├── Makefile
├── plundervolt.c               // Plundervolt Undervolting Lib
├── plundervolt.h               // (Software Undervolting)
└── README.md                   // This file
```

## VoltPillager Controlling Library 
### Related files
`hwvolt.c` and `hwvolt.h`

### Functions
* `int init_hw_volt()`
    * `trigger_serial`, the serial port for sending DTR trigger, usually `/dev/ttyS0`
    * `teensy_serial`, USB-Serial device to teensy
    * `teensy_baudrate`, USB-Serial baudrate to teensy, corresponding to 
    `Serial.begin();` config in `voltpillager-teensy` firmware. Currently configuration is `115200`
	
* `int configure_glitch_with_delay()`
    * send configuration of glitch to voltpillager and arm the glitch, voltage in V, time in uS, see Table 2 of the paper
    * `repeat`: N
    * `v1`: V<sub>p</sub> 
    * `d1`: TT<sub>p</sub> 
    * `v2`: V<sub>f</sub> 
    * `d2`: TT<sub>f</sub> 
    * `delay_before_glitch_us`: T<sub>d</sub>
	* Note: TT<sub>p</sub> + voltage changing time = <T<sub>p</sub>>, so it can be a negative, same for TT<sub>f</sub> 

* `int destroy_hw_volt();`
    * cleanup and free memory
    
* `int hwvolt_arm()`
    * Arm the glitch, need to be called everytime after the glitch is triggered 

* `TRIGGER_SET`
    * When using DTR trigger, this marco will set DTR pin to LOW
    * When using TEENSY USB trigger, this marco will send an empty line through USB(Serial)
    
* `TRIGGER_RST`
    * Only effective when using DTR trigger
    * Reset DTR pin to HIGH

* `TRIGGER_PULSE`
    * Only effective when using DTR trigger
    * Generate a pulse on DTR pin, ----__----

* `int teensy_serial_trigger();`
    * Internal use only, see `hwvolt.h` for detail
* `int configure_glitch()`
    * Internal use only, see `hwvolt.h` for detail

## Trigger Type Selection
Trigger type can be selected by setting `TRIGGER_SEL` variable when invoking makefile.
* set `export TRIGGER_SEL="-D TRIGGER_TEENSY"` to use USB trigger of teensy(USB Serial)
* set `export TRIGGER_SEL="-D TRIGGER_DTR"` to use DTR trigger
* If this variable is not set, DTR trigger is used by default

## Plundervolt Undervolting Library 
To to quick test with software undervolting, we packet voltage changing functionality of plundervolt into a library. 
### Requirement
`sudo modprobe msr`

### Functions
* `int init_plundervolt()`
    * Initialize plundervolt (software undervolting)

* `uint64_t wrmsr_value()`
    * Generate msr config for undervolting voltage by X mV on plane Y

* `void voltage_change()`
    * Write undervolting configuration to msr
    * e.g. `voltage_change(wrmsr_value(-200,2));` will undervolt the CPU by -200mV on plane 2

* `void reset_voltage()`
    * Reset the voltage to normal
    
* `void destroy_plundervolt()`
    * Clean up and free memory

