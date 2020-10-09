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


MKIMAGE=/openmiko/build/buildroot-2016.02/output/build/uboot-openmiko/tools/mkimage
if [ ! -f "/usr/sbin/mkimage" ]; then
	ln -s $MKIMAGE /usr/sbin/mkimage
fi

# Pack up the image so it can be installed using the factory demo.bin
# method. Use the default firmware for some pieces.
#
# Maximum sizes
#                                                                                                                                                                      
# "kernel" - 0x200000 or 2,097,152 bytes
# "rootfs" - 0x350000 or 3,473,408 bytes
# "driver" - 0xa0000 or 655,360 bytes
# "appfs" - 0x4a0000 or 4,849,664 bytes


KERNEL="$IMAGES/uImage.lzma"
PADDED_KERNEL="$IMAGES/uImagePadded.lzma"

ROOTFSFILE="$IMAGES/rootfs.tar.xz"
JFFSROOTIMG="$IMAGES/rootfs.jffs2"


KERNEL_MAXSIZE=$((16#200000))
ROOTFS_MAXSIZE=$(( $((16#350000)) + $((16#a0000)) + $((16#4a0000)) ))
FLASH_MAXSIZE=$(( $KERNEL_MAXSIZE + $ROOTFS_MAXSIZE ))


# Checks

KERNEL_BYTES=$(wc -c < "$KERNEL")
if [ $KERNEL_BYTES -ge $KERNEL_MAXSIZE ]; then
	echo "Error: Kernel image must be less than $KERNEL_MAXSIZE. It is $KERNEL_BYTES."
	exit 1
fi



# Pad the kernel to max size
cp $KERNEL $PADDED_KERNEL
truncate -s $KERNEL_MAXSIZE $PADDED_KERNEL


JFFS2=$BASE_DIR/host/usr/sbin/mkfs.jffs2 

rm -rf $IMAGES/jffsroot
mkdir -p $IMAGES/jffsroot
cp $ROOTFSFILE $IMAGES/jffsroot

$JFFS2 -d $IMAGES/jffsroot -e 0x8000 -o $JFFSROOTIMG



ROOTFS_BYTES=$(wc -c < "$JFFSROOTIMG")
if [ $ROOTFS_BYTES -ge $ROOTFS_MAXSIZE ]; then
	echo "Error: rootfs image must be less than $ROOTFS_MAXSIZE. It is $ROOTFS_BYTES."
	exit 1
fi


# Combine kernel and rootfs into one file and pad it to total size of flash
KERNEL_AND_ROOT="$IMAGES/kernel_and_root.bin"
cat $PADDED_KERNEL $JFFSROOTIMG > $KERNEL_AND_ROOT
truncate -s $FLASH_MAXSIZE $KERNEL_AND_ROOT


# Make an image for flashing
OUTFILE="${RELEASE_DIR}/openmiko_firmware.bin"
$MKIMAGE -A MIPS -O linux -T firmware -C none -a 0 -e 0 -n jz_fw -d $KERNEL_AND_ROOT $OUTFILE


cp $OUTFILE $RELEASE_DIR/demo.bin
echo "Firmware created: $RELEASE_DIR/demo.bin"

cp $JFFSROOTIMG $RELEASE_DIR


cat << EOF

Build and release complete.

Kernel ==> $KERNEL ($KERNEL_BYTES / $KERNEL_MAXSIZE )
RootFS (tar.xz) ==> $ROOTFSFILE ($ROOTFS_BYTES / $ROOTFS_MAXSIZE )
RootFS Image ==> $JFFSROOTIMG

EOF

