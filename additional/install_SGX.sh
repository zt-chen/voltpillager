#!/bin/bash

# This script will install intel SGX driver and intel SGX SDK

set -e

script_path=$(pwd)
sgx_sdk_version="sgx_2.8"
sgx_driver_version="sgx_driver_2.6"

# SGX SDK requirements
sudo apt-get install build-essential ocaml ocamlbuild automake autoconf libtool wget python libssl-dev git cmake perl -y
# SGX PSW requirements
sudo apt-get install libssl-dev libcurl4-openssl-dev protobuf-compiler libprotobuf-dev debhelper cmake reprepro unzip -y

git submodule init
git submodule update

OS_ID=$(lsb_release -si | tr '[:upper:]' '[:lower:]')
OS_REL=$(lsb_release -sr)
OS_STR=$OS_ID$OS_REL

# ----------------------------------------------------------------------
# Install SGX Driver
git clone https://github.com/intel/linux-sgx-driver.git
cd linux-sgx-driver
git checkout $sgx_driver_version

sudo apt-get -yqq install linux-headers-$(uname -r)
make
sudo mkdir -p "/lib/modules/"`uname -r`"/kernel/drivers/intel/sgx"
sudo cp isgx.ko "/lib/modules/"`uname -r`"/kernel/drivers/intel/sgx"
sudo sh -c "cat /etc/modules | grep -Fxq isgx || echo isgx >> /etc/modules"
sudo /sbin/depmod
sudo /sbin/modprobe isgx

echo "SGX driver succesfully installed"

# ----------------------------------------------------------------------
echo "[ building SDK ]"
cd $script_path
git clone https://github.com/intel/linux-sgx.git
cd linux-sgx
git checkout $sgx_sdk_version
./download_prebuilt.sh
make sdk_install_pkg -j`nproc`

echo "[ installing SDK system-wide ]"
cd linux/installer/bin/
sudo ./sgx_linux_x64_sdk_*.bin << EOF
no
/opt/intel
EOF
cd ../../../

# ----------------------------------------------------------------------
echo "[ building PSW ]"
make psw_install_pkg -j`nproc`

echo "[ installing PSW/SDK system-wide ]"
cd linux/installer/bin/

if [ -e /opt/intel/sgxpsw/uninstall.sh ]
then
    sudo /opt/intel/sgxpsw/uninstall.sh
fi
sudo ./sgx_linux_x64_psw_*.bin

echo "SGX SDK succesfully installed!"
