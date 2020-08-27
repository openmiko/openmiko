#!/bin/sh

chmod -R 755 /system

cd /tmp
if [ -f /system/.upgrade ]; then
    cd /backupa
    echo "init upgrading!!!!!!!!!!!!"
    sh ./upgrade.sh
    rm /system/.upgrade
fi

if [ -f /configs/.upgrade ]; then
    echo "new init upgrading!!!!!!!!!!!!"
    sh /configs/merge_upgrade.sh &
    exit 0
fi

export LD_LIBRARY_PATH=/tmp:$LD_LIBRARY_PATH
echo "nameserver 8.8.8.8" >> /tmp/resolv.conf

echo "################################"
echo "######## this is dafang ########"
echo "################################"


insmod /driver/tx-isp.ko isp_clk=100000000
insmod /driver/exfat.ko
insmod /driver/sample_motor.ko
insmod /driver/sinfo.ko
insmod /driver/sample_pwm_core.ko
insmod /driver/sample_pwm_hal.ko
insmod /driver/audio.ko
insmod /driver/8189es.ko

echo 47 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio47/direction
echo 1 > /sys/class/gpio/gpio47/value

echo 61 > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio61/direction
echo 1 > /sys/class/gpio/gpio61/value
sleep 1
echo 0 > /sys/class/gpio/gpio61/value


#wpa_supplicant -Dwext -i wlan0 -c /system/etc/wpa_supplicant.conf -B
#udhcpc -i wlan0 -s /system/etc/udhcpc.script -q

#ifconfig eth0 up
#udhcpc -i eth0 -s /system/etc/udhcpc.script -q
ifconfig eth0 10.10.10.10 netmask 255.255.255.0
route add default gw 10.10.10.1

# open ircut
#cp /system/bin/setir /tmp/
#config ip address

###/system/bin/carrier-server --st=imx322
###/system/bin/singleBoadTest
/system/bin/test_UP &
/system/bin/sdkshellcalltool &
/system/bin/hl_client &
/system/bin/iCamera &
/system/bin/dongle_app &

for i in $(seq 1 2)
do
    sleep 10
    isRunCam=$(ps | grep iCamera | grep -v "grep" | wc -l)
    if [ 1 -eq $isRunCam ]; then
        echo "iCamera is Running"
        exit
    fi
done

echo "iCamera not running"
echo "restore system from backup"
touch /configs/.upgrade
touch /configs/.fsrepair

sleep 1
reboot

