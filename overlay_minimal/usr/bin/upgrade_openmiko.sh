#!/bin/bash

# exit when any command fails
set -e

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

killall mjpg_streamer || echo "Process was not running."
killall autonight || echo "Process was not running."
killall videocapture || echo "Process was not running."
killall v4l2rtspserver || echo "Process was not running."

cd /tmp

curl -L https://github.com/openmiko/openmiko/releases/download/$GIT_TAG/rootfs.jffs2 -o /tmp/rootfs.jffs2
curl -L https://github.com/openmiko/openmiko/releases/download/$GIT_TAG/uImage.lzma -o /tmp/uImage.lzma

echo "Flashing root file system (rootfs)"
flash_eraseall /dev/mtd2
dd if=/tmp/rootfs.jffs2 of=/dev/mtdblock2
echo "Wrote rootfs.jffs2"

echo "Flashing kernel image"
flash_eraseall /dev/mtd1
dd if=/tmp/uImage.lzma of=/dev/mtdblock1
echo "Wrote uImage.lzma"

echo "Image flashed. Reboot is required."