#!/bin/bash

# Create the cpio root filesystem that is embedded in the kernel
# This is a minimal root filesystem to bootstrap the bigger rootfs

cd /src/initramfs_root
mkdir -p /src/release
find . | cpio -H newc -o > /src/release/initramfs.cpio
