#!/bin/bash
set -e

# This script is expected to be run inside the development container
# It copies from /src files that have been changed for buildroot purposes

cd /openmiko/build/buildroot-2016.02

# Copy over custom packages removing standard ones we don't want to use

# ffmpeg doesn't seem to build out of the box
rm -rf /openmiko/build/buildroot-2016.02/package/ffmpeg

cp -r /src/custompackages/package/* /openmiko/build/buildroot-2016.02/package/

patch -p1 < /src/patches/add_fp_no_fused_madd.patch
patch -p1 < /src/patches/libv4l_add_lpthread.patch

# Replace the default buildroot config files with our custom ones
cp /src/config/buildroot.config .config
cp /src/config/busybox.config package/busybox
cp /src/config/uClibc-ng.config package/uclibc

# We want to use specific sources so copy these into the download directory
mkdir -p dl
cp /src/legacy_src/kernel-3.10.14.tar.xz dl/
