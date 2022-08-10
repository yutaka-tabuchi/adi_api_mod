#!/bin/bash

BASEDIR=`dirname $0`
WORKDIR=`pwd`

if [ -z "$TARGET_ADDR" ]; then
        echo "usage TARGET_ADDR=XXX.XXX.XXX.XXX ./setup.sh"
        exit
fi

source /tools/Xilinx/Vivado/2020.1/settings64.sh

cd $BASEDIR/src
./adrf6780.sh 
./lmx2594_120_a.sh 
./lmx2594_110_LO.sh 
./ad5328-quel2-adrf6780.sh*
./ad5328-quel2-heater.sh*
./rfswitch.sh

cd $WORKDIR

pwd
if [ -n "$BITFILE" ]; then
  vivado -mode batch -source $BASEDIR/config.tcl
fi

cd $BASEDIR/v1.0.6/src
pwd
AD9082_CHIP=1 ./hello
AD9082_CHIP=0 ./hello

cd $WORKDIR

cd $BASEDIR/v1.0.6/src
pwd
AD9082_CHIP=1 ./hello
AD9082_CHIP=0 ./hello

cd $BASEDIR/src
# start internal parameter update for MicroBlaze software
./udpsendrecv userctrl w 1

cd $WORKDIR

echo "script:" ${BASEDIR}
echo "target:" ${TARGET_ADDR}
if [ -n "$BITFILE" ]; then
  echo "FPGA:", $BITFILE
fi
