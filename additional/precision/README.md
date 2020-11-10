# 5.2 Timing Precision - Jitter Test for Triggers
As there is no way to measure the latency between the calling of TRIGGER_SET and the actual trigger in teensy, we do jitter measurement in a tricky way. 

We keep sending trigger on PC program in a loop and measure a signal on teensy. The SPI send is replaced with sending this signal. As each loop **should** have constant timing, so by measuring the period of the signal send by teensy, we will be able to calculate the jitter. 

This jitter can be caused by:

* OS Scheduling (Can be improved by using realtime kernel)
* Kernel Driver (E.g, serial send will wait for a while for the program to fill the buffer)
* The trigger handling code on Teensy Side
    * Serial trigger has a receive buffer, data will not be available until the buffer is filled.(https://www.pjrc.com/teensy/td_serial.html)
    * Serial trigger will parse the text send on serial in a loop but not interrupt. E.g Teensy will wait until receiving a full byte of character and check if that is the trigger character.
* Measurement
    * The inaccuracy of timer on X86 -> inaccuracy sleep()


## System Configuration
* Kernel Parameter

    ```GRUB_CMDLINE_LINUX_DEFAULT="quiet splash intel_pstate=disable intel_idle.max_cstate=0 isolcpus=1"```

* Bind to core 1
    ```	
  // assign the process to be running on core 1
  cpu_set_t my_set;        // Define your cpu_set bit mask.
  CPU_ZERO(&my_set);       // Initialize it all to 0, i.e. no CPUs selected.
  CPU_SET(1, &my_set);     // set the bit that represents core 1.
  sched_setaffinity(0, sizeof(cpu_set_t), &my_set); // Set affinity of tihs process to
  print_affinity();       // CPU core mask, indicating which core this process will be running on.
  ```
* Real-time scheduling attributes

    ```
    sudo chrt --rr 99 ./jitter_test
    ```
* Flash jitter_test_teensy firmware to teensy

* Run jitter_test on host PC

## Host Program
`jitter_test.c`

This program will send trigger in a loop with constant delay

modify `TRIGGER_SEL` in `Makefile` to choose between `TRIGGER_TEENSY`(USB) or `TRIGGER_DTR`

## Teensy Firmware for Trigger Test
Folder: `jitter_test_teensy`

This is a simplified version of teensy_code with SPI sending commands removed and always armed

When the Teensy received the trigger(DTR/serial), it will set `TRIGGER_OSCI`(PIN 17).

## RIGOL DS1074Z Capture Configuration
* adjust the scale and keep 500MSa/S
* Save memory using csv format (probably will be faster if save waveform) 

The data captured are in `oscilloscope_capture.zip`, unzip it to `oscilloscope_capture` folder for data processing

Note: For USB trigger, the TRIGGER_RST is empty, so the period will be shorter than DTR trigger 

## Data Processing
`calc_jitter.py` is used for processing the oscilloscope data (in `oscilloscope_capture` folder) and calculate jitter. The period calculated for each iteration of the loop is saved in `periods_dtr.csv` and `periods_usb.csv`
