#!/bin/sh
./udpsendrecv lmx2594 w $1 2D C8F2 # R45 [5:0] OUTB_PWR=1F
./udpsendrecv lmx2594 w $1 2C 3220 # R44 [7] OUTB_PD=0(=Active)
