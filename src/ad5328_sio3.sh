#!/bin/sh

./udpsendrecv ad5328 w 8 00C # range 0--3.3V (buffered,Vref)
./udpsendrecv ad5328 w 0 800 # output-A 1.65V
./udpsendrecv ad5328 w 1 800 # output-B 1.65V
./udpsendrecv ad5328 w 2 800 # output-C 1.65V
./udpsendrecv ad5328 w 3 800 # output-D 1.65V
./udpsendrecv ad5328 w 4 800 # output-E 1.65V
./udpsendrecv ad5328 w 5 800 # output-F 1.65V
./udpsendrecv ad5328 w 6 800 # output-G 1.65V
./udpsendrecv ad5328 w 7 800 # output-H 1.65V
#./udpsendrecv ad5328 w 0 C00 # output-A 1.65V
#./udpsendrecv ad5328 w 1 C00 # output-B 1.65V
#./udpsendrecv ad5328 w 2 C00 # output-C 1.65V
#./udpsendrecv ad5328 w 3 C00 # output-D 1.65V
#./udpsendrecv ad5328 w 4 C00 # output-E 1.65V
#./udpsendrecv ad5328 w 5 C00 # output-F 1.65V
#./udpsendrecv ad5328 w 6 C00 # output-G 1.65V
#./udpsendrecv ad5328 w 7 C00 # output-H 1.65V
./udpsendrecv ad5328 w A 002 # /LDAC single update(必要なのでは？？？ by miyo)
