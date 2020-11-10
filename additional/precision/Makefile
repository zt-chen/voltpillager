# try to do some autodetecting
UNAME := $(shell uname -s)

ifeq "$(UNAME)" "Darwin"
	OS=macosx
endif
ifeq "$(OS)" "Windows_NT"
	OS=windows
endif
ifeq "$(UNAME)" "Linux"
	OS=linux
endif


#################  Mac OS X  ##################################################
ifeq "$(OS)" "macosx"

EXE_SUFFIX=

ARCHS=   -arch i386 -arch x86_64
CFLAGS+= $(ARCHS)
CFLAGS += -mmacosx-version-min=10.6
CFLAGS_MONGOOSE=  -I./mongoose -pthread -g 
LIBS+=	 $(ARCHS)

endif

#################  Windows  ##################################################
ifeq "$(OS)" "windows"

EXE_SUFFIX=.exe

CFLAGS_MONGOOSE = -I./mongoose -mthreads

endif


#################  Common  ##################################################
#export TRIGGER_SEL = -D TRIGGER_TEENSY
export TRIGGER_SEL = -D TRIGGER_DTR

#SLEEP_SEL = -D SLEEP_NOP
SLEEP_SEL = -D SLEEP_TSC

HWLIB_DIR=../../lib/
LIB_ARDUINO_SC=$(HWLIB_DIR)/arduino-serial/
LIB_VOLT= $(HWLIB_DIR)

INCLUDES	+= -I $(LIB_ARDUINO_SC) -I $(LIB_VOLT)
LDLIBS		+= -L $(LIB_ARDUINO_SC) -L $(LIB_VOLT)

CFLAGS 		= $(TRIGGER_SEL) $(INCLUDES) $(LDLIBS) $(LIBS) -O0 -Wall -mrdrnd
LDFLAGS		+= -lhwvolt -larduino-serial -pthread

all: jitter_test

lib:
	$(info -----Building hwvolt library-----)
	$(MAKE) -C $(HWLIB_DIR)

jitter_test: lib jitter_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o jitter_test$(EXE_SUFFIX) jitter_test.c

clean:
	rm -f $(OBJ) jitter_test jitter_test.exe *.o *.a
	rm -f *.s
	$(MAKE) -C $(HWLIB_DIR) clean


