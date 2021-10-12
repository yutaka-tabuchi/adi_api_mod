#!/bin/sh
for i in 0 1 2 3 4 5 6 7
do
	./udpsendrecv lmx2594 r $i 22 0 # R34 [2:0] PLL_N[18:16]
	./udpsendrecv lmx2594 r $i 24 0 # R36 PLL_N[15:0]
	./udpsendrecv lmx2594 r $i 26 0 # R38 PLL_DEN[31:16]
	./udpsendrecv lmx2594 r $i 27 0 # R39 PLL_DEN[15:0]
	./udpsendrecv lmx2594 r $i 2A 0 # R42 PLL_NUM[31:16]
	./udpsendrecv lmx2594 r $i 2B 0 # R43 PLL_NUM[15:0]
done
