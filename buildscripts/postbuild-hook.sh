#!/bin/bash
set -e

echo "Peforming post rootfs build hooks"

# The path to the images output directory is passed as the first argument
DEFAULT_IMAGE_DIR="/openmiko/build/buildroot-2016.02/output/images"
IMAGES=${1:-$DEFAULT_IMAGE_DIR}
BASE_DIR=${BASE_DIR:-/openmiko/build/buildroot-2016.02/output}

# The environment variables BR2_CONFIG, HOST_DIR, STAGING_DIR,
# TARGET_DIR, BUILD_DIR, BINARIES_DIR and BASE_DIR are defined

# Put the kernel image on the tftp server
if [[ -d /var/lib/tftpboot ]]; then
	cp $IMAGES/uImage.lzma /var/lib/tftpboot
fi

RELEASE_DIR=/src/release
mkdir -p $RELEASE_DIR


cd /src
REVISION_HASH=`git rev-parse --quiet --short HEAD`
NOW=`date +'%Y-%m-%d_%H_%M_%S'`

# Copy the kernel image and bootloader to releases
cp $IMAGES/uImage.lzma $RELEASE_DIR
cp $IMAGES/u-boot-lzo-with-spl.bin $RELEASE_DIR



# Pack up the image so it can be installed using the factory demo.bin
# method. Use the default firmware for some pieces.
#
# Usage: packer.py [OPTIONS] [KERNEL] [ROOTFS] [DRIVER] [APPFS] [OUTFILE]

# Maximum sizes
#                                                                                                                                                                      
# "kernel" - 0x200000 or 2,097,152 bytes
# "rootfs" - 0x350000 or 3,473,408 bytes
# "driver" - 0xa0000 or 655,360 bytes
# "appfs" - 0x4a0000 or 4,849,664 bytes


KERNEL="$IMAGES/uImage.lzma"

# ROOTFS Must be a Squashfs filesystem
ROOTFS=/src/stock_firmware/wyzecam_v2/4.9.6.156/flash/rootfs.bin
if [[ -f "$IMAGES/rootfs.squashfs" ]]; then
	echo "Using custom rootfs/squashfs instead of stock rootfs"
	ROOTFS="$IMAGES/rootfs.squashfs"
fi

DRIVER=/src/stock_firmware/wyzecam_v2/4.9.6.156/flash/driver.bin


APPFS=/src/stock_firmware/wyzecam_v2/4.9.6.156/flash/appfs.bin
if [[ -f "$IMAGES/appfs.bin" ]]; then
	echo "Using custom appfs (jffs2)"
	APPFS="$IMAGES/appfs.bin"
fi

# OUTFILE="${RELEASE_DIR}/openmiko_${NOW}_${REVISION_HASH}_demo.bin"
OUTFILE="${RELEASE_DIR}/openmiko_firmware.bin"


MKIMAGE=/openmiko/build/buildroot-2016.02/output/build/uboot-openmiko/tools/mkimage
if [ ! -f "/usr/sbin/mkimage" ]; then
	ln -s $MKIMAGE /usr/sbin/mkimage
fi

cat << EOF

Kernel ==> $KERNEL
RootFS ==> $ROOTFS
Driver ==> $DRIVER
AppFS  ==> $APPFS

EOF

/usr/bin/python /src/utilities/packer.py $KERNEL $ROOTFS $DRIVER $APPFS $OUTFILE

cp $OUTFILE $RELEASE_DIR/demo.bin
echo "Firmware created: $RELEASE_DIR/demo.bin"

cd $DEFAULT_IMAGE_DIR

if [[ -f rootfs.ext2 ]]; then
	echo "Compressing rootfs.ext2"
	tar czvf $RELEASE_DIR/rootfs.ext2.tar.gz rootfs.ext2
fi

