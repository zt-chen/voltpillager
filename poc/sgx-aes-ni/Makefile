#export TRIGGER_SEL = -D TRIGGER_TEENSY
export TRIGGER_SEL = -D TRIGGER_DTR
HWLIB_DIR	=../../lib/
LIB_ARDUINO_SC=$(HWLIB_DIR)/arduino-serial/
LIB_VOLT	= $(HWLIB_DIR)
INCLUDE		+= -I $(LIB_ARDUINO_SC) -I $(LIB_VOLT)
LDLIBS		+= -L $(LIB_ARDUINO_SC) -L $(LIB_VOLT)
LDFLAGS		+= -lhwvolt -larduino-serial

ifeq ($(SGX_SDK),)
    SGX_SDK          = /opt/intel/sgxsdk
endif
export SGX_SDK

ENCLAVE              = Enclave
SUBDIRS              = $(ENCLAVE)

CC                   = gcc
AS                   = gcc
LD                   = gcc

ifeq ($(M32), 1)
	CFLAGS   = -m32 -DM32=$(M32)
	LDFLAGS  = -m32 
else
	LIB_SUFX = 64
endif

LDFLAGS		    += -lncurses
CFLAGS              += $(TRIGGER_SEL) -fPIC -fno-stack-protector -fno-builtin -fno-jump-tables \
                       -fno-common -Wno-attributes -g -D_GNU_SOURCE -O0 -mavx2 -march=haswell
INCLUDE             += -I$(SGX_SDK)/include/
LDFLAGS             +=  -lencl_proxy -lsgx_urts \
                       -lsgx_uae_service -pthread $(SUBDIRS:%=-L %) -L$(SGX_SDK)/lib$(LIB_SUFX)/ \
                       $(LDLIBS)

SOURCES              = $(shell ls *.c)
CXXSOURCES              = $(shell ls *.cpp)
OBJECTS              = $(SOURCES:.c=.o)
CXXOBJECTS              = $(CXXSOURCES:.cpp=.o)
OUTPUT               = app

BUILDDIRS            = $(SUBDIRS:%=build-%)
CLEANDIRS            = $(SUBDIRS:%=clean-%)

.SILENT:

all: $(OUTPUT)
	
run: clean all
	sudo $(URTS_LD_LIBRARY_PATH) ./$(OUTPUT) 500 -255

debug: clean all
	sudo `which sgx-gdb` ./$(OUTPUT)

lib:
	$(info -----Building hwvolt library-----)
	$(MAKE) -C $(HWLIB_DIR)

$(OUTPUT): $(BUILDDIRS) $(OBJECTS) lib
	echo "$(INDENT)[LD]" $(OBJECTS) $(LIBS) -o $(OUTPUT) 
	$(LD) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT) 

%.o : %.c
	echo "$(INDENT)[CC] " $<
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

%.o : %.S
	echo "$(INDENT)[AS] " $<
	$(AS) $(INCLUDE) -c $< -o $@

clean: $(CLEANDIRS)
	echo "$(INDENT)[RM]" $(OBJECTS) $(OUTPUT)
	rm -f $(OBJECTS) $(OUTPUT)
	$(MAKE) -C ./Enclave clean
	$(MAKE) -C $(HWLIB_DIR) clean

$(BUILDDIRS):
	echo "$(INDENT)[===] $(@:build-%=%) [===]"
	$(MAKE) -C $(@:build-%=%) INDENT+="$(INDENT_STEP)" M32=$(M32) curr-dir=$(curr-dir)/$(@:build-%=%)

$(CLEANDIRS):
	echo "$(INDENT)[===] $(@:clean-%=%) [===]"
	
