//
// Created by Zitai Chen on 10/05/2020.
//

#include "hwvolt.h"
#include "arduino-serial-lib.h"
#include <errno.h>   	 // ERROR Number Definitions
#include <fcntl.h>   	 // File Control Definitions
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/serial.h>


// Make it global to save some clock cycles
int fd_teensy = -1;
int fd_trigger = -1;

// Using DTR/RTS as trigger
int RTS_flag = TIOCM_RTS;
int DTR_flag = TIOCM_DTR;
char buf[BUFMAX];


void hwvolt_error(char* msg)
{
	fprintf(stderr, "%s\n",msg);
	exit(-1);
}

// Arm the glitch
int hwvolt_arm(){
	int rc = serialport_write(fd_teensy, "arm\n");
	if(rc==-1) {
		hwvolt_error("error writing");
		return -1;
	}
	return 0;
}
// Arm the glitch
int set_debug(){
	int rc = serialport_write(fd_teensy, "debug\n");
	if(rc==-1) {
		hwvolt_error("error writing");
		return -1;
	}
	return 0;
}
// send configuration commands to teensy
// Internal use only, or need to call arm after using this
int configure_glitch(int repeat, float v1, int d1, float v2, int d2, float v3){

	char buf[BUFMAX];
	memset(buf,0,BUFMAX);  //
	if( fd_teensy == -1 ) hwvolt_error("serial port not opened");
	sprintf(buf, ("%i %1.4f %i %1.4f %i %1.4f\n"), repeat, v1, d1, v2, d2, v3);
	printf("send: %s", buf);
	int rc = serialport_write(fd_teensy, buf);
	if(rc==-1) {
		hwvolt_error("error writing");
		return -1;
	}
	memset(buf,0,BUFMAX);  //
	serialport_read_lines(fd_teensy, buf, EOLCHAR, BUFMAX, TIMEOUT, 3);
	printf("resp: %s", buf);
	return 0;
}
// send configuration commands to teensy, also send delay time and arm the glitch
int configure_glitch_with_delay(int repeat, float v1, int d1, float v2, int d2, float v3, int delay_before_glitch_us){
	// send delay command
	char buf[BUFMAX];
	memset(buf,0,BUFMAX);  //
	sprintf(buf, ("delay %i\n"), delay_before_glitch_us);
	printf("send: %s", buf);
	int rc = serialport_write(fd_teensy, buf);
	if(rc==-1) {
		hwvolt_error("error writing");
		return -1;
	}
	memset(buf,0,BUFMAX);  //
	serialport_read_lines(fd_teensy, buf, EOLCHAR, BUFMAX, TIMEOUT,2);
	printf("resp:%s",buf);

	configure_glitch(repeat, v1, d1, v2, d2, v3);
	rc = serialport_write(fd_teensy, "arm\n");
	if(rc==-1) {
		hwvolt_error("error writing");
		return -1;
	}
	memset(buf,0,BUFMAX);  //
	serialport_read_lines(fd_teensy, buf, EOLCHAR, BUFMAX, TIMEOUT,2);
	printf("resp:%s",buf);
	return 0;
}
// Trigger over Teensy USB serial connection
int teensy_serial_trigger()
{
	int n = write(fd_teensy, "\n", 1);
	if( n!=1 ) {
		fprintf(stderr, "Ret: %d\n", n);
		hwvolt_error("error writing\n");
		return -1;
	}
	return 0;
}
// Initialize the connection
int init_hw_volt(char* const trigger_serial, char* const teensy_serial, int teensy_baudrate){
// If using hardware trigger (onboard DTR), then enable it
#ifdef TRIGGER_DTR
	//// Configure on-board serial (to be used as trigger)
	fd_trigger = open(trigger_serial, O_RDWR | O_NOCTTY );
	if( fd_trigger==-1 ) {
		printf("Trigger serial: could not open port\n");
		return -1;
	}
	printf("Trigger serial: opened port %s\n",trigger_serial);


	// Create new termios struc, we call it 'tty' for convention
	struct termios tty;
	memset(&tty, 0, sizeof tty);

	// Read in existing settings, and handle any error
	if(tcgetattr(fd_trigger, &tty) != 0) {
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	}

	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	//tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
	//tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

	tty.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	// Set in/out baud rate to be 9600
	cfsetispeed(&tty, B38400);
	cfsetospeed(&tty, B38400);


	struct serial_struct kernel_serial_settings;
	int r = ioctl(fd_trigger, TIOCGSERIAL, &kernel_serial_settings);
	if (r >= 0) {
		kernel_serial_settings.flags |= ASYNC_LOW_LATENCY;
		r = ioctl(fd_trigger, TIOCSSERIAL, &kernel_serial_settings);
		if (r >= 0) printf("set linux low latency mode\n");
	}

	tcsetattr(fd_trigger, TCSANOW, &tty);
	if( tcsetattr(fd_trigger, TCSAFLUSH, &tty) < 0) {
		perror("init_serialport: Couldn't set term attributes");
		return -1;
	}
#endif

	//// Always Init teensy serial
	// Open the port
	// If it is open, close it and reopen
	if( fd_teensy!=-1 ) {
		serialport_close(fd_teensy);
		printf("closed port %s\n",teensy_serial);
	}

	fd_teensy = serialport_init(teensy_serial, teensy_baudrate);
	if( fd_teensy==-1 ) {
		hwvolt_error("Teensy Serial: couldn't open port");
		return -1;
	}
	printf("Teensy serial: opened port %s\n",teensy_serial);
	serialport_flush(fd_teensy);

	return 0;
}

// close the file/port properly
int destroy_hw_volt(){
	close(fd_teensy);
#ifdef TRIGGER_DTR
	close(fd_trigger);
#endif
	return 0;
}
