# This is the initramfs embedded file system skeleton

The reason for this skeleton is so you can put files in here that will be embedded in the kernel image itself. Be careful not to put too many files as the kernel needs to fit in about 2MB of memory. The exact number is detailed in buildscripts/postbuild-hook.sh but it is 0x200000 or 2,097,152 bytes.

The skeleton is used because you cannot commit into git the empty directories or special dev nodes which are required to be created in the initramfs root tree. Thus the script src/buildscripts/make_initramfs.sh serves to create these dynamically. It then cpio's up the whole thing and puts it in

```
CONFIG_INITRAMFS_SOURCE="/openmiko/build/buildroot-2016.02/output/images/initramfs.cpio"
```
# Rebuilding

To rebuild the initramfs and the firmware together:

```
/src/buildscripts/linux_prebuild_hook.sh
make rootfs-initramfs
make linux-rebuild
make
```