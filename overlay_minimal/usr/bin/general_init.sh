#!/bin/bash
set -x

# /etc/dropbear is a symbolic link to /var/run/dropbear
# Remove it so overlays in that directory will work
rm -f /etc/dropbear
mkdir -p /etc/dropbear

logger -s -t general_init "Setting up SDCard access"

echo 43 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio43/direction
sleep 3

if [ -e /dev/mmcblk0p1 ]; then
	mkdir -p /sdcard
	mount -t vfat /dev/mmcblk0p1 /sdcard -o rw,umask=0000,dmask=0000
	sleep 1
	echo "Mount /sdcard successful"

	# Write log files to the sdcard
	mkdir -p /sdcard/var/log

	# Current state is /var/log -> ../tmp
	# At this point /tmp is empty (not sure why)
	rm /var/log
	ln -s /sdcard/var/log /var/log
fi;

logger -s -t general_init "Mounting flash partitions"

mkdir -p /config
mount -t jffs2 /dev/mtdblock3 /config

if [ $? -ne 0 ]
then
	logger -s -t general_init "Error mounting /dev/mtdblock3 on /config"
	exit 1
fi

# Should implement an optimization here to check for each file
# so we don't wear out flash chip
if [ -d "/sdcard/config" ]
then
	logger -s -t general_init "Copying files from sdcard to flash storage"
	find /sdcard/config -type f -exec sh -c 'SOURCE={};TARGET=${SOURCE#/sdcard};install -D $SOURCE $TARGET' \; -print
fi

logger -s -t general_init "Copying files from flash storage to ram"

# Overlay the config flash with the rootfs in memory
find /config/overlay -type f -exec sh -c 'SOURCE={};TARGET=${SOURCE#/config/overlay};install -D $SOURCE $TARGET' \; -print

logger -s -t general_init "sdcard and config flash partition mounted"
