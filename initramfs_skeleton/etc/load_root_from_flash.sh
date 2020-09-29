#!/bin/sh

echo "Starting full root buildout"

mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t tmpfs tmpfs /tmp

mkdir -p /dev/pts
mount -t devpts devpts /dev/pts

echo "Mounted core filesystems"

mkdir -p /jffsroot
mount -t jffs2 /dev/mtdblock2 /jffsroot

echo "Mounted jffsroot"

mkdir -p /newroot
mount -t devtmpfs newroot /newroot
tar -xvf /jffsroot/rootfs.tar.gz -C /newroot

exec switch_root /newroot /sbin/init
