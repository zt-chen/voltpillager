# 5.1 Faulting Multiplications
## Command Line Arguments
```
Usage: glitch_controller -b <bps> -p <serialport> [OPTIONS]
  -h  --help                    Print this help message
  -b  --baud=baudrate           Baudrate (bps) of Arduino (default 115200)
  -p  --port=teensySerial       Serial port Teensy is connected to
  -s  --send=string             Send string to Arduino
  -S  --sendline=string         Send string with newline to Arduino
  -i  --stdinput                Use standard input
  -r  --receive                 Receive string from Arduino & print it out
  -n  --num=num                 Send a number as a single byte
  -F  --flush                   Flush serial port buffers for fresh reading
  -e  --eolchar=char            Specify EOL char for reads (default '\n')
  -t  --timeout=millis          Timeout for reads in millisecs (default 5000)
  -q  --quiet                   Don't print out as much info

  -g  --glitch                  Do the voltage glitch
  -d  --delay                   Delay after trigger (Td) in uS
      --iter                    Iterations of undervolting (assembly)
      --num_p                   Number of svid packets (N)
      --pre_volt                Voltage before the glitch (Vp)
      --pre_delay               Delay before the glitch (TTp)
      --glitch_voltage          Glitch voltage (Vf)
      --rst_volt                Reset voltage (Vn)
      --rst_delay               Reset delay (TTf)

      --target_id               The target function id (check the targets h file) 0:MUL, 1:CMP
      --targer_name             The name of the targer, e.g the name of "mul_target" is "mul"
      --calc_op1                Operand 1
      --calc_op2                Operand 2
      --calc_max_or_fixed_op1
      --calc_max_or_fixed_op2
      --calc_op1_min
      --calc_op2_min
      --calc_thread_num         Numbers of thread
      --retries                 Retries of every configuration
```


## Trigger Type Selection
```
export TRIGGER_SEL = -D TRIGGER_TEENSY
export TRIGGER_SEL = -D TRIGGER_DTR
```

## Example result
```
// MUL (5.1 Faulting Multiplication)
// Run on i3-7100-Z170 at 2Ghz
sudo ./glitch_controller -b 115200 -p /dev/ttyACM0 -d 1000 --retries 1000 --num_p 1 --pre_volt 0.83 --pre_delay 35 --glitch_voltage 0.64 --rst_volt 0.83 --rst_delay "-30" --target_name mul --iter 10000000 --calc_thread_num 1 --calc_op1 0xae0000 --calc_op2 0x18 -g

------   [MUL] CALCULATION ERROR DETECTED   ------
 > Iterations  	 : 00090236
 > Operand 1   	 : 0000000000ae0000
 > Operand 2   	 : 0000000000000018
 > Correct     	 : 0000000010500000
 > Result      	 : 0000000010500000
 > Result      	 : 000000000c500000
 > xor result  	 : 000000001c000000


// CMP (6.1 Delayed-Write, Initial PoC)
// Run on i3-7100-Z170 at 3Ghz
sudo ./glitch_controller -b 115200 -p /dev/ttyACM0 -d 200 --retries 1000 --num_p 1 --pre_volt 0.95 --pre_delay 35 --glitch_voltage 0.76 --rst_volt 0.95 --rst_delay "-30" --target_name cmp --iter 10000000 --calc_thread_num 1 -g

------   CALCULATION ERROR DETECTED   ------
 > OpCode CMP (if)
 > Iterations  	 : 00097702
 > Operand 1   	 : 000000005fcf6faf
 > Operand 2   	 : 000000005fcf6faf
 // system crash
```
#### Note
This is also a voltage glitching framework for non-enclave codes. 
You can write you new target functions in `target_functions.c/.h` and add them at line31 of `glitch_controller.c`


