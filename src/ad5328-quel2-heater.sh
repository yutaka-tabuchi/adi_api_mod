#!/bin/sh

for i in 0 1 3 4
do
./udpsendrecv ad5328 w $i 8 00C # range 0--3.3V (buffered,Vref)
./udpsendrecv ad5328 w $i 0 A00 # output-A 1.65V
./udpsendrecv ad5328 w $i 1 A00 # output-B 1.65V
./udpsendrecv ad5328 w $i 2 A00 # output-C 1.65V
./udpsendrecv ad5328 w $i 3 A00 # output-D 1.65V
./udpsendrecv ad5328 w $i 4 A00 # output-E 1.65V
./udpsendrecv ad5328 w $i 5 A00 # output-F 1.65V
./udpsendrecv ad5328 w $i 6 A00 # output-G 1.65V
./udpsendrecv ad5328 w $i 7 A00 # output-H 1.65V
./udpsendrecv ad5328 w $i A 002 # /LDAC single update(必要なのでは？？？ by miyo)
done
