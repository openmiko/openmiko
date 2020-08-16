#!/bin/bash
set -e

echo "Peforming post rootfs build hooks"

/usr/sbin/service tftpd-hpa start

# The path to the images output directory is passed as the first argument
DEFAULT_IMAGE_DIR="/openmiko/_build/buildroot-2016.02/output/images"
IMAGES=${1:-$DEFAULT_IMAGE_DIR}

# The environment variables BR2_CONFIG, HOST_DIR, STAGING_DIR,
# TARGET_DIR, BUILD_DIR, BINARIES_DIR and BASE_DIR are defined

# Put the kernel image on the tftp server
cp $IMAGES/uImage.lzma /var/lib/tftpboot

rm -rf /sharedfiles/rootfs
mkdir -p /sharedfiles/rootfs
cp $IMAGES/rootfs.tar /sharedfiles/rootfs
cd /sharedfiles/rootfs
tar xvf rootfs.tar

