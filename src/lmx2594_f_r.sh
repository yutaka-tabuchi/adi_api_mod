#!/bin/sh

./udpsendrecv lmx2594 r $1 22 0 # R34 [2:0] PLL_N[18:16]
./udpsendrecv lmx2594 r $1 24 0 # R36 PLL_N[15:0]
./udpsendrecv lmx2594 r $1 26 0 # R38 PLL_DEN[31:16]
./udpsendrecv lmx2594 r $1 27 0 # R39 PLL_DEN[15:0]
./udpsendrecv lmx2594 r $1 2A 0 # R42 PLL_NUM[31:16]
./udpsendrecv lmx2594 r $1 2B 0 # R43 PLL_NUM[15:0]
