for i in 0 1 2 3 4 5 6 7 8 9
do
TARGET_ADDR=10.5.0.1 ./udpsendrecv adrf6780 w 0 0 4000
TARGET_ADDR=10.5.0.1 ./udpsendrecv adrf6780 w 0 0 0000
TARGET_ADDR=10.5.0.1 ./udpsendrecv adrf6780 r 0 3 0000
TARGET_ADDR=10.5.0.1 ./udpsendrecv adrf6780 r 0 3 0000
done
