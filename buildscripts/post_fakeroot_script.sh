#!/bin/bash
set -e

echo "Executing pre filesystem image creation script"

# The environment variables BR2_CONFIG, HOST_DIR, STAGING_DIR,
# TARGET_DIR, BUILD_DIR, BINARIES_DIR and BASE_DIR are defined


DEFAULT_IMAGE_DIR="/openmiko/build/buildroot-2016.02/output/images"
BASE_DIR=${BASE_DIR:-/openmiko/build/buildroot-2016.02/output}
IMAGES="${BASE_DIR}/images"
HOST_DIR=${HOST_DIR:-/openmiko/build/buildroot-2016.02/output/host}
TARGET_DIR=${TARGET_DIR:-/openmiko/build/buildroot-2016.02/output/target}

# Prepare the CPIO initramfs image
# INITRAMFS_ROOT="${BASE_DIR}/initramfs_root"

# mkdir -p $INITRAMFS_ROOT/{bin,sbin,etc,proc,sys}
# cp -r $BASE_DIR/target/bin $INITRAMFS_ROOT/
# cd $INITRAMFS_ROOT
# find . | cpio -H newc -o > ../initramfs.cpio


# To save space on the root file system move some big files to /apps
# Package /apps up to a jffs2 file system then delete it so it doesn't get bundled

mkdir -p $TARGET_DIR/apps
if [[ -f $TARGET_DIR/usr/sbin/wpa_supplicant ]]; then
	mv $TARGET_DIR/usr/sbin/wpa_supplicant $TARGET_DIR/apps/
fi


mkdir -p $TARGET_DIR/apps/lib/modules/3.10.14/kernel/drivers/net/wireless/
if [[ -d $TARGET_DIR/lib/modules/3.10.14/kernel/drivers/net/wireless/rtl818x ]]; then
	mv $TARGET_DIR/lib/modules/3.10.14/kernel/drivers/net/wireless/rtl818x $TARGET_DIR/apps/lib/modules/3.10.14/kernel/drivers/net/wireless/
fi


mkdir -p $TARGET_DIR/apps/usr/lib
if [[ -f $TARGET_DIR/usr/lib/libfreetype.so.6.12.2 ]]; then
	mv $TARGET_DIR/usr/lib/libfreetype.so.6.12.2 $TARGET_DIR/apps/usr/lib
fi

if [[ -f $TARGET_DIR/usr/lib/libmp3lame.so.0.0.0 ]]; then
	mv $TARGET_DIR/usr/lib/libmp3lame.so.0.0.0 $TARGET_DIR/apps/usr/lib
fi

if [[ -f $TARGET_DIR/usr/lib/libopus.so.0.5.2 ]]; then
	mv $TARGET_DIR/usr/lib/libopus.so.0.5.2 $TARGET_DIR/apps/usr/lib
fi

$HOST_DIR/usr/sbin/mkfs.jffs2 -e 32KiB -d $TARGET_DIR/apps -o $IMAGES/appfs.bin


rm -rf ${TARGET_DIR}/apps/*

# Remove the stamp target installed to avoid an issue on next make
rm -f ${BASE_DIR}/build/v4l2rtspserver-v0.1.3/.stamp_target_installed
rm -f ${BASE_DIR}/build/wpa_supplicant-2.5/.stamp_target_installed
rm -f ${BASE_DIR}/build/linux-custom/.stamp_target_installed
rm -f ${BASE_DIR}/build/freetype-2.6.2/.stamp_target_installed
rm -f ${BASE_DIR}/build/lame-3.99.5/.stamp_target_installed
rm -f ${BASE_DIR}/build/opus-1.1.2/.stamp_target_installed

