

# About the root file system and initramfs

Inside `make linux-menuconfig` there are two options that are critical to the initial embedded file system.

1. Initial RAM filesystem and RAM disk (initramfs/initrd) support
2. (/src/initramfs.cpio) Initramfs source file(s)

The first is needed to make sure we are able to load a rootfs that is embedded in the kernel.
The second `INITRAMFS_SOURCE` has a number of caveats in how it is configured.

In particular the `INITRAMFS_SOURCE` should be a cpio archive to avoid any issues. One of the issues I ran into was pointing it to a directory (i.e. /src/initramfs_root). However this resulted in a number of files not being included. This was most likely due to how INITRAMFS_SOURCE is being interpreted and it requires a specific way to format it. To avoid this issue we stick to cpio archives so we can easily verify what is in them.


## Verifying that embedded file system

A combination of the utility `binwalk` and `cpio` can help extract and verify the initial embedded root file system.

```
binwalk --extract uImage.lzma
cd _uImage.lzma.extracted

binwalk --extract 40
cd _40.extracted

cpio -i < 5244B8
```
