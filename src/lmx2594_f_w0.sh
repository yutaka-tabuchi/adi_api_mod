#!/bin/sh
for i in 0 1 2 3 4 5 6 7
do
	./udpsendrecv lmx2594 w $i 22 0000 # R34 [2:0] PLL_N[18:16]
	./udpsendrecv lmx2594 w $i 24 0078 # R36 PLL_N[15:0]
	./udpsendrecv lmx2594 w $i 26 0000 # R38 PLL_DEN[31:16]
	./udpsendrecv lmx2594 w $i 27 0064 # R39 PLL_DEN[15:0]
	./udpsendrecv lmx2594 w $i 2A 0000 # R42 PLL_NUM[31:16]
	./udpsendrecv lmx2594 w $i 2B 0032 # R43 PLL_NUM[15:0]
	./udpsendrecv lmx2594 w $i 00 2618
done
