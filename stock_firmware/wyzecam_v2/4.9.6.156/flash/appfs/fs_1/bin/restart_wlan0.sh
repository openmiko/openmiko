#!/bin/sh
while [ `ifconfig|grep wlan0|wc -l` = 1 ]
do
ifconfig wlan0 down
echo "down"
done
sleep 0.5
while [ `ifconfig|grep wlan0|wc -l` = 0 ]
do
ifconfig wlan0 up
echo "up"
sleep 0.5
done