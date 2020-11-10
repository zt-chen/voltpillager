# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

include ../../config.mk



CFLAGS=$(shell pkg-config oehost-$(C_COMPILER) --cflags)
CXXFLAGS=$(shell pkg-config oehost-$(CXX_COMPILER) --cflags)
LDFLAGS=$(shell pkg-config oehost-$(CXX_COMPILER) --libs)
INCDIR=$(shell pkg-config oehost-$(C_COMPILER) --variable=includedir)


#export TRIGGER_SEL = -D TRIGGER_TEENSY
export TRIGGER_SEL = -D TRIGGER_DTR

# Change it to your lib path
HWLIB_DIR	=/home/tryagain/voltpillager/lib

LIB_ARDUINO_SC=$(HWLIB_DIR)/arduino-serial/
LIB_VOLT	= $(HWLIB_DIR)
INCLUDE		+= -I $(LIB_ARDUINO_SC) -I $(LIB_VOLT)
LDLIBS		+= -L $(LIB_ARDUINO_SC) -L $(LIB_VOLT)
LDFLAGS		+= -lhwvolt -larduino-serial

CFLAGS 		+= $(TRIGGER_SEL) $(INCLUDE)
CXXFLAGS 	+= $(TRIGGER_SEL) $(INCLUDE)

all: build

lib:
	$(info -----Building hwvolt library-----)
	$(MAKE) -C $(HWLIB_DIR)

build: lib
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../fileencryptor.edl --untrusted \
		--search-path $(INCDIR) \
		--search-path $(INCDIR)/openenclave/edl/sgx
	$(CC) -g -c $(CFLAGS) fileencryptor_u.c
	$(CXX) $(TRIGGER_SEL) -g -c $(CXXFLAGS) host.cpp
	$(CXX) -o file-encryptorhost host.o fileencryptor_u.o $(LDLIBS) $(LDFLAGS)

clean:
	rm -f file-encryptorhost fileencryptor_u.* fileencryptor_args.h *.o ../out.decrypted ../out.encrypted

