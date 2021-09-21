#!/bin/bash

cd /openmiko/build/buildroot-2016.02
/src/buildscripts/linux_prebuild_hook.sh
make rootfs-initramfs
make linux-rebuild
make

