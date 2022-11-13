#!/bin/bash
set -x

. /usr/bin/libgpio.sh
gpio_select_gpiochip 0

SD_PARTITION=/dev/mmcblk0p1
SD_FILESYSTEM=vfat
FILESYSTEMS=(vfat exfat ext4 ext3 ext2 ext4dev)

if [[ -f /etc/openmiko.conf ]]; then
	. /etc/openmiko.conf

elif [[ -f /config/overlay/etc/openmiko.conf ]]; then
	. /config/overlay/etc/openmiko.conf
fi

function run_fsck() {
  local SD_FILESYSTEM=$1
  FSCK_FLAGS="-p"
  if [[ "$SD_FILESYSTEM" == "vfat" ]]; then
	  FSCK_FLAGS="-p -w"
  elif [[ "$SD_FILESYSTEM" == "vfat" ]]; then
	  FSCK_FLAGS="-p -w"
  fi

  logger -s -t general_init "Attempting fsck.$SD_FILESYSTEM"

	if command -V fsck."$SD_FILESYSTEM" > /dev/null; then
    fsck -t "$SD_FILESYSTEM" $FSCK_FLAGS "$SD_PARTITION"
    local RETURN_CODE=$?
    logger -s -t general_init "fsck.$SD_FILESYSTEM returned $RETURN_CODE"
    return $RETURN_CODE
	fi

  logger -s -t general_init "fsck.$SD_FILESYSTEM not found"
	return 1
}

function try_fscks() {
  local SD_FILESYSTEM=$1
  run_fsck "$SD_FILESYSTEM"
  local RETURN_CODE=$?

  while [ $RETURN_CODE -ne 0 ] && [ ${#FILESYSTEMS[@]} -ne 0 ]; do
    NEW_FILESYSTEMS=()

    for FILESYSTEM in "${FILESYSTEMS[@]}"; do
      [[ $FILESYSTEM != $SD_FILESYSTEM ]] && NEW_FILESYSTEMS+=($FILESYSTEM)
    done

    FILESYSTEMS=("${NEW_FILESYSTEMS[@]}")
    unset NEW_FILESYSTEMS

    SD_FILESYSTEM="${FILESYSTEMS[0]}"
    run_fsck "$SD_FILESYSTEM"
    RETURN_CODE=$?
  done
  
  return $RETURN_CODE
}

function get_next_filesystem() {
  local LAST_FILESYSTEM=$1
  NEW_FILESYSTEMS=()

  for FILESYSTEM in "${FILESYSTEMS[@]}"; do
    [[ $FILESYSTEM != $LAST_FILESYSTEM ]] && NEW_FILESYSTEMS+=($FILESYSTEM)
  done

  FILESYSTEMS=("${NEW_FILESYSTEMS[@]}")
  unset NEW_FILESYSTEMS

  echo "${FILESYSTEMS[0]}"
}

function mount_sdcard() {
    # Attempting filesystem defined in config
    if ! mount -t "$SD_FILESYSTEM" "$SD_PARTITION" /sdcard -o rw; then
      # Attempt to auto-detect filesystem
  	  if ! mount "$SD_PARTITION" /sdcard -o rw; then
  	    return 1
  	  fi
    fi
}

# /etc/dropbear is a symbolic link to /var/run/dropbear
# Remove it so overlays in that directory will work
rm -f /etc/dropbear
mkdir -p /etc/dropbear

logger -s -t general_init "Setting up SDCard access"

setup_sdcard_access() {

  if ! try_fscks "$SD_FILESYSTEM"; then
    logger -s -t general_init "fsck failed"
  fi

	mkdir -p /sdcard

  if mount_sdcard; then
    logger -s -t general_init "mount /sdcard successful"
	else
    logger -s -t general_init "mount /sdcard failed"
  fi

  sleep 1

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
