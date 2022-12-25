#!/bin/sh
set -x

. /usr/bin/libgpio.sh
gpio_select_gpiochip 0

SD_PARTITION=/dev/mmcblk0p1
SD_FILESYSTEM=vfat
if [[ -f /etc/openmiko.conf ]]; then
	. /etc/openmiko.conf

elif [[ -f /config/overlay/etc/openmiko.conf ]]; then
	. /config/overlay/etc/openmiko.conf
fi


# /etc/dropbear is a symbolic link to /var/run/dropbear
# Remove it so overlays in that directory will work
rm -f /etc/dropbear
mkdir -p /etc/dropbear

logger -s -t general_init "Setting up SDCard access"

setup_sdcard_access() {
	mkdir -p /sdcard
	# mount -t vfat $SD_PARTITION /sdcard -o rw,umask=0000,dmask=0000
	mount -t $SD_FILESYSTEM $SD_PARTITION /sdcard -o rw
	sleep 1
	echo "Mount /sdcard successful"

	# Write log files to the sdcard
	mkdir -p /sdcard/var/log

	# Current state is /var/log -> ../tmp
	# At this point /tmp is empty (not sure why)
	rm /var/log
	ln -s /sdcard/var/log /var/log
}


# Determine if sdcard access needs to be setup
# On WyzeCam Pan units the /dev/mmcblk0p1 is available without having
# to export pin 43. However on WyzeCams you need to export 43
# for the mmc devices to show up

if [ -e $SD_PARTITION ]; then
	setup_sdcard_access
else
	# If the device doesn't exist then either the sdcard is not present or we need to export the pin
	gpio_export '43'
	gpio_direction_output '43'
	sleep 3

	# If after exporting the device exists then setup
	if [ -e $SD_PARTITION ]; then
		setup_sdcard_access
	else
		# If it still doesn't exist then unexport and continue
		gpio_unexport '43'
	fi;
fi;


OPENMIKO_CONFIG_FILE=/sdcard/config/overlay/etc/openmiko.conf
if [[ -f $OPENMIKO_CONFIG_FILE ]]; then
	. $OPENMIKO_CONFIG_FILE
fi

logger -s -t general_init "Mounting flash partitions"

mkdir -p /config
mount -t jffs2 /dev/mtdblock3 /config

if [ $? -ne 0 ]
then
	logger -s -t general_init "Error mounting /dev/mtdblock3 on /config"
	exit 1
fi

clear_config_partition() {
	if [[ "$CLEAR_CONFIG_PARTITION" == "1" ]]; then
		echo "Wiping /config"
		rm -rf /config/*
		echo "Wipe complete. Make sure you remove the CLEAR_CONFIG_PARTITION flag."
	fi	
}
clear_config_partition


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
