wpa_supplicant -D nl80211 -iwlan0 -c/system/bin/wpa.conf -B &
sleep 3
udhcpc -i wlan0 -p /var/run/udhcpc.pid -b &
