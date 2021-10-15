#!/bin/sh
./udpsendrecv lmx2594 w $1 22 0000 # R34 [2:0] PLL_N[18:16]
./udpsendrecv lmx2594 w $1 24 $2 # R36 PLL_N[15:0]
./udpsendrecv lmx2594 w $1 26 0000 # R38 PLL_DEN[31:16]
./udpsendrecv lmx2594 w $1 27 0064 # R39 PLL_DEN[15:0]
./udpsendrecv lmx2594 w $1 2A 0000 # R42 PLL_NUM[31:16]
./udpsendrecv lmx2594 w $1 2B 0000 # R43 PLL_NUM[15:0]
./udpsendrecv lmx2594 w $1 00 6418
