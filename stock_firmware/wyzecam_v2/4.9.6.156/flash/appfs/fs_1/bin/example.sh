#!/bin/sh

ifname=$1
supplicant_stat=$2

echo "-------------------------------$ifname $supplicant_stat------------------------------------"

echo "$supplicant_stat" > /tmp/networkStation.txt

case "$supplicant_stat" in
	CONNECTED)
	killall udhcpc
	udhcpc -i wlan0 -p /var/run/udhcpc.pid -b &
	;;
	
	DISCONNECTED)
	wpa_cli -p /var/run/wpa_supplicant -i wlan0 disconnect
	wpa_cli -p /var/run/wpa_supplicant -i wlan0 reconnect
	;;

esac

