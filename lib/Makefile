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
#TRIGGER_SEL_LOCAL = -D TRIGGER_TEENSY
TRIGGER_SEL_LOCAL = -D TRIGGER_DTR
TRIGGER_SEL := $(if $(TRIGGER_SEL),$(TRIGGER_SEL),$(TRIGGER_SEL_LOCAL))

LIB_ARDUINO_SC=arduino-serial/
INCLUDES += -I $(LIB_ARDUINO_SC)
CFLAGS = $(TRIGGER_SEL) $(INCLUDES) -O0 -Wall -mrdrnd

all: libhwvolt libplundervolt arduinoserial
	$(info Trigger type: $(TRIGGER_SEL))

arduinoserial: $(LIB_ARDUINO_SC)/arduino-serial-lib.c $(LIB_ARDUINO_SC)/arduino-serial-lib.h
	$(CC) $(CFLAGS) -std=gnu99 -c $(LIB_ARDUINO_SC)/arduino-serial-lib.c -o libarduino-serial.a

libhwvolt: hwvolt.c hwvolt.h
	$(CC)  $(CFLAGS)  -std=gnu99 -c hwvolt.c -o libhwvolt.a

libplundervolt: plundervolt.c plundervolt.h
	$(CC) $(CFLAGS) -std=gnu99 -c plundervolt.c -o libplundervolt.a

clean:
	rm -f $(OBJ) *.o *.a
	rm -f *.s

