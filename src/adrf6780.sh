for i in 0 1 2 3 4 5 6 7 8 9
do
./udpsendrecv adrf6780 w $i 0 4000
./udpsendrecv adrf6780 w $i 0 0000
./udpsendrecv adrf6780 w $i 3 0167
./udpsendrecv adrf6780 w $i 5 0400
done
