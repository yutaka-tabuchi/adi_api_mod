#!/bin/sh

./adrf6780.sh 
./lmx2594_120_a.sh 
./lmx2594_120_LO_ctrl.sh 
./lmx2594_120_LO_readout.sh 
#./lmx2594_96_LO.sh 
#./lmx2594_iso_LO.sh 
./ad5328.sh 
./rfswitch.sh
