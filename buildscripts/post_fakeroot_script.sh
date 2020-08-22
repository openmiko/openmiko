#!/bin/bash
set -e

echo "Peforming post fakeroot"

# BR2_ROOTFS_POST_FAKEROOT_SCRIPT

# The environment variables BR2_CONFIG, HOST_DIR, STAGING_DIR,
# TARGET_DIR, BUILD_DIR, BINARIES_DIR and BASE_DIR are defined
printenv


DEFAULT_IMAGE_DIR="/openmiko/build/buildroot-2016.02/output/images"
IMAGES="${BASE_DIR}/images"


$HOST_DIR/usr/sbin/mkfs.jffs2 -e 32KiB -d $TARGET_DIR/apps -o $IMAGES/appfs.bin

rm -rf ${TARGET_DIR}/apps
# Remove the stamp target installed to avoid an issue on next make
rm ${BASE_DIR}/build/v4l2rtspserver-v0.1.3/.stamp_target_installed
