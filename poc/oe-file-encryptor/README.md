# 5.1 End-to-end attack OE file encryptor
### Prepare the openenclave environment
```
sudo apt install pkg-config
wget https://github.com/Kitware/CMake/releases/download/v3.18.2/cmake-3.18.2-Linux-x86_64.sh
chmod +x cmake-3.18.2-Linux-x86_64.sh
./cmake-3.18.2-Linux-x86_64.sh

git clone https://github.com/openenclave/openenclave.git
cd openenclave
git checkout 7060b014b6a7af86eb6329ca49e07296dda9fe3e
git submodule update --recursive --init
mkdir build/
cd build/
../../cmake-3.18.2-Linux-x86_64/bin/cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
make -j4
../../cmake-3.18.2-Linux-x86_64/bin/cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/opt/openenclave ..
sudo make install
source /opt/openenclave/share/openenclave/openenclaverc
```

### Build
The project requires the open enclave SDK to be installed in the system. (Tested with commit 7060b014b6a7af86eb6329ca49e07296dda9fe3e)

This folder needs to be placed in the samples folder of the open enclave repository.

`HWLIB_DIR` in host/Makefile need to be changed to the path of undervolting lib, 

e.g:
`HWLIB_DIR	=/home/tryagain/voltpillager/lib`

To build the project run `make` in this folder.


### Usage
```
./host/file-encryptorhost <parameters>

Parameters:

-i : iterations
-r : svid_repeats
-v : trg_voltage
-p : pre_voltage
-d : pre_delay
-a : post_voltage
-b : post_delay
-g : pre_glitch_delay
```

The parameters of the undervolting have default values as constants in the host/host.cpp file.

Check the values of the trigger serial port and the teensy serial port and change them if needed.


The program will initialize an encryption enclave with an "random" and unknown  key and it will start encrypting data. It will perform undervolting while encrypting based on the configure parameters until a fault was detected. It will gradually decrease the target voltage to increase the probability of a fault occuring. When it identifies a fault it will save the plaintext, the correct ciphertext and the faulty ciphertext in the directory "fault_results". Each fault will have a prefix based on the time so that it will not overwrite previous faults. 

If multiple blocks are used the utility [aes-faulty-round-finder](../../additional/aes-faulty-block-finder/) can help to identify the first faulty block.
