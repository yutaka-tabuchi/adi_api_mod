#!/bin/bash
./udpsendrecv adrf6780 w $1 0 4000
./udpsendrecv adrf6780 w $1 0 0000
./udpsendrecv adrf6780 w $1 3 0167
./udpsendrecv adrf6780 w $1 5 $2
