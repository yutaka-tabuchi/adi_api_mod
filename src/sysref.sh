#!/bin/bash

for i in 0 1 2 3 4 5 6 7 8 9
do
./udpsendrecv lmx2594 w $i 00 6612 # R6 [14] VCO_PHASE_SYNC=1
sleep 1
./udpsendrecv lmx2594 w $i 00 6610
sleep 1
./udpsendrecv lmx2594 r $i 00 0
./udpsendrecv lmx2594 w $i 47 004D # R71 [7:5]SYSREF_DIV_PRE=2='Divided by 2', [3]SYSREF_EN=1, [2]SYSREF_REPEAT=1
./udpsendrecv lmx2594 w $i 3A 0201 # R58 [15]IGNORE=0, [14]HYST=0, [13:12]INPIN_LVL=0=Vin/4, [11:9]FMT=2='sysrefreq=LVDS'
./udpsendrecv lmx2594 w $i 2E 07FE # R46 [1:0] OUTB_MUX=2
./udpsendrecv lmx2594 w $i 2D C8DF # R45 [5:0] OUTB_PWR=1F
./udpsendrecv lmx2594 w $i 2C 3220 # R44
./udpsendrecv lmx2594 w $i 00 6618 # R6 [14] VCO_PHASE_SYNC=1
./udpsendrecv lmx2594 r $i 00 0
done
