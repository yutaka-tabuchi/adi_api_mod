#!/bin/bash

if [ -z "$TARGET_ADDR" ]; then
        echo "usage TARGET_ADDR=XXX.XXX.XXX.XXX ./setup.sh"
        exit
fi

source /tools/Xilinx/Vivado/2020.1/settings64.sh
#export TARGET_ADDR=10.5.0.14

cd $HOME/adi_api_mod/src
./adrf6780.sh 
./lmx2594_120_a.sh 
./lmx2594_110_LO.sh 
./ad5328.sh 
./rfswitch.sh

cd $HOME

#cd $HOME
#export BITFILE=$HOME/bin/069414.bit
#vivado -mode batch -source $HOME/config.tcl

cd $HOME/ADI_api_106/src
AD9082_CHIP=1 ./hello
cd $HOME/ADI_api_106/src
AD9082_CHIP=0 ./hello

cd $HOME/ADI_api_106/src
AD9082_CHIP=1 ./hello
cd $HOME/ADI_api_106/src
AD9082_CHIP=0 ./hello

cd $HOME

echo "setup done for" ${TARGET_ADDR}

