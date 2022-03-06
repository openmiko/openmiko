#!/bin/bash
set -e

# This script is expected to be run inside the development container
# It copies from /src files that have been changed for buildroot purposes

cd /openmiko/build/buildroot-2016.02

# Copy over custom packages removing standard ones we don't want to use

# ffmpeg doesn't seem to build out of the box so use our own
rm -rf /openmiko/build/buildroot-2016.02/package/ffmpeg


# Use our own mjpg-streamer package
rm -rf /openmiko/build/buildroot-2016.02/package/mjpg-streamer

# Old ncurses doesn't support xterm-256color so use updated one
rm -rf /openmiko/build/buildroot-2016.02/package/ncurses

rm -rf /openmiko/build/buildroot-2016.02/package/lighttpd

rm -rf /openmiko/build/buildroot-2016.02/package/logrotate


#cp -r /src/custompackages/package/ffmpeg /openmiko/build/buildroot-2016.02/package/
cp -r /src/custompackages/package/* /openmiko/build/buildroot-2016.02/package/


# Avoid FPU bug on XBurst CPUs
patch -p1 < /src/patches/add_fp_no_fused_madd.patch

# Video for Linux package needs -lpthread (only needed if I compile it)
patch -p1 < /src/patches/libv4l_add_lpthread.patch

# Add LINUX_PRE_BUILD_HOOKS to create embedded initramfs file
patch -p1 < /src/patches/linux_makefile.patch

# Replace the default buildroot config files with our custom ones
# The linux configuration is set inside the ingenic_t20_defconfig
# using BR2_LINUX_KERNEL_CUSTOM_CONFIG_FILE

cp /src/config/ingenic_t20_defconfig configs/
cp /src/config/busybox.config package/busybox
cp /src/config/uClibc-ng.config package/uclibc


# We want to use specific sources so copy these into the download directory
mkdir -p dl
cp /src/legacy_src/kernel-3.10.14.tar.xz dl/



# Loads up our custom configuration
make ingenic_t20_defconfig

# We just loaded it but these commands are how you save it back (here for reference)
# Technically should be a no-op
make savedefconfig BR2_DEFCONFIG=/src/config/ingenic_t20_defconfig
# make linux-update-defconfig

# Start the build process
cd /openmiko/build/buildroot-2016.02

make sqlite
make

