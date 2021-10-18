#!/bin/bash

function do_flash {
	# $1 = path to file (/tmp/rootfs.jffs2)
	# $2 = MTD device number (0, 1, 2)
	if [[ ! -n $1 || ! -n $2 ]]; then
		echo "Invalid parameters given to do_flash."
		return 1
	fi
	
	FILE=$1
	DEVICE=/dev/mtd${2}
	BLOCKDEVICE=/dev/mtdblock${2}
	if [[ ! -s $FILE ]]; then
		echo "ERROR: File $FILE does not exist or is 0 bytes."
		return 1
	fi
	
	FILE_SIZE=$(wc -c < $FILE)
	echo "Flashing contents of $(basename $FILE) to $DEVICE..."
	flash_eraseall $DEVICE
	dd if=$FILE of=$BLOCKDEVICE
	if [[ $? -ne 0 ]]; then
		echo "ERROR: Failed to flash device $DEVICE from file $(basename $FILE)."
		return 1
	fi
	echo "Wrote $(basename $FILE) to $DEVICE."
	
	# Read back flash contents and checksum
	echo "Comparing contents of $DEVICE to $(basename $FILE)... this may take a while"
	dd if=$BLOCKDEVICE conv=sync,noerror bs=1 count=$FILE_SIZE 2>/dev/null | cmp $FILE
	if [[ $? -ne 0 ]]; then
		echo "ERROR: New flash contents on $DEVICE do not match the source file $FILE."
		return 1
	else
		echo "Verified the contents of $DEVICE match the source file $FILE."
	fi
}

if [[ $# -eq 0 ]] ; then
    echo './upgrade_openmiko.sh <GIT_TAG>'
    exit 1
fi

read -p "Are you sure you want to flash new firmware? " -n 1 -r
echo    # (optional) move to a new line
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
	# handle exits from shell or function but don't exit interactive shell
    [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1
fi


GIT_TAG="$1"

echo -e "\nStopping services..."
killall mjpg_streamer || echo "Process was not running."
killall autonight || echo "Process was not running."
killall videocapture || echo "Process was not running."
killall v4l2rtspserver || echo "Process was not running."
echo "Services stopped."

cd /tmp

echo -e "\nDownloading updated images from GitHub..."
curl -fL https://github.com/openmiko/openmiko/releases/download/$GIT_TAG/rootfs.jffs2 -o /tmp/rootfs.jffs2
if [[ $? -ne 0 ]]; then
	echo "Failed to download OpenMiko $GIT_TAG rootfs.jffs2 from GitHub."
	exit 1
fi

curl -fL https://github.com/openmiko/openmiko/releases/download/$GIT_TAG/uImage.lzma -o /tmp/uImage.lzma
if [[ $? -ne 0 ]]; then
	echo "Failed to download OpenMiko $GIT_TAG uImage.lzma from GitHub."
	exit 1
fi
echo "Images downloaded successfully."

# Doing a while loop here to use break in case of failure
while true; do
	echo -e "\nFlashing root file system (rootfs)"
	# Flash rootfs.jffs2 to /dev/mtd2
	if do_flash /tmp/rootfs.jffs2 2; then
		true
	else
		break
	fi

	echo -e "\nFlashing kernel image (uImage.lzma)"
	# Flash uImage.lzma to /dev/mtd1
	if do_flash /tmp/uImage.lzma 1; then
		true
	else
		break
	fi
	
	echo -e "\nOpenMiko image flashed successfully. Reboot is required.\n"
	exit 0
done

echo -e "\nERROR: Upgrade failed.  Your system may not survive a reboot!  Troubleshoot and retry the upgrade.\n"
exit 1
