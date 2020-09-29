#!/bin/bash

# Create the cpio root filesystem that is embedded in the kernel
# This is a minimal root filesystem to bootstrap the bigger rootfs

ROOTFS_DIR=/openmiko/build/buildroot-2016.02/output/initramfs_root

rm -rf $ROOTFS_DIR

/src/buildscripts/make_initramfs.sh $ROOTFS_DIR

cd $ROOTFS_DIR

find . | cpio -H newc -o > /openmiko/build/buildroot-2016.02/output/images/initramfs.cpio
