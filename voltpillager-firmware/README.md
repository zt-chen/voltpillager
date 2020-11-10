# 4.1 VoltPillager Teensy Firmware
## Requirement
* Teensydrino [Link](https://www.pjrc.com/teensy/td_download.html), tested on v1.51
* TeensyTimerTool library v0.1.8 (Can be installed using Library Manager)

## Compile configuration
* CPU Speed: 600MHz
* Optimize: "Fastest"

## Supported Commands
* < N > < V<sub>p</sub> > < TT<sub>p</sub> > < V<sub>f</sub> > < TT<sub>f</sub> > < V<sub>n</sub> >
	* This command is for setting the glitch parameters, voltage in V, time in uS 
	* <TT<sub>p</sub>> + voltage changing time = <T<sub>p</sub>>, so it can be a negative value, same for <TT<sub>f</sub>>
* arm
	* Arm the glitch
	* After the glitch is armed, teensy will wait for DTR trigger (falling edge) from PIN 4, or USB(serial) trigger. 
* delay < T<sub>d</sub> >
	* Delay time after receiving the trigger, in uS

	

See Table 2 of paper for glitch parameters