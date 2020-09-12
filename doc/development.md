# Development


To setup the development environment run `docker-compose up -d` to automatically pull down a container with buildroot.

You can also run `docker pull openmiko/openmiko:latest` to get the latest container. 

The container image is about 8GB. The reason for this is because the artifacts are all baked in.
This reduces the amount of time needed to get up and running.

Once you have the container up and running you can shell into it using:

`docker exec -it -e TERM <containerid> bash` or
`docker-compose exec builder bash`

To build the firmware change you directory to:
`/openmiko/build/buildroot-2016.02` and run `make`.

This should output something like the following:

```
Peforming post rootfs build hooks
Image Name:   jz_fw
Created:      Sat Sep 12 15:14:33 2020
Image Type:   MIPS Linux Firmware (uncompressed)
Data Size:    11075584 Bytes = 10816.00 kB = 10.56 MB
Load Address: 00000000
Entry Point:  00000000
Firmware created: /src/release/demo.bin

Build and release complete.

Kernel ==> /openmiko/build/buildroot-2016.02/output/images/uImage.lzma (2092002 / 2097152 )
RootFS ==> /openmiko/build/buildroot-2016.02/output/images/rootfs.tar.xz (8466824 / 8978432 )
```




## Bootloader



## Kernel

The kernel is based on Linux 3.10.14. The old kernel version is due to what the manufacturer supports. Porting the changes to a newer kernel would be welcome.



CONFIG_CMDLINE
---

This is set because we are unable to overwrite the default bootloader that comes from the manufacturer in an easy manner. The reason is that the stock bootloader's `sdupdate` command checks for a `demo.bin` that only contains a kernel and root filesystem to flash to NAND. There is no way to update the bootloader itself using this method. Thus whatever kernel we use must use its own arguments to boot from sdcard since we can't change the default arguments.

The commandline chosen boots from `/dev/mmcblk0p2`. Thus we must have a two partition sdcard with the 2nd being an `ext2/3/4` partition.

```
CONFIG_CMDLINE="console=ttyS1,115200n8 mem=96M@0x0 ispmem=8M@0x6000000 rmem=24M@0x6800000 init=/linuxrc root=/dev/mmcblk0p2 rootfstype=ext4 rootwait rw mtdparts=jz_sfc:256k(boot),2048k(kernel),3392k(root),640k(driver),4736k(appfs),2048k(backupk),640k(backupd),2048k(backupa),256k(config),256k(para),-(flag)"
```

## RootFS




## Buildroot

Rebuild skeleton for rootfs and make changes without having to rebuild everything

```

rm -rf target
mkdir -p target/sbin
mkdir -p target/lib
mkdir -p target/usr/lib
cp -a staging/sbin/* target/sbin/
cp -a staging/lib/* target/lib/
cp -a staging/usr/lib/* target/usr/lib/
rm -f build/.root
find . -name ".stamp_target_installed*" -print | xargs rm -f
```





### Building busybox static binary for use in initramfs

Since we use initramfs and bundle in a starting root filesystem into the kernel we need some small utilities to populate it to start. The file `config/busybox-min.conf` is used for a minimal busybox static binary compilation.

You can copy this file over to the `output/build/busybox-version`

After the binary is built copy it to `initramfs_root/bin`

Then remake the initramfs filesystem:

```
make rootfs-initramfs
```



H264

https://stackoverflow.com/questions/24884827/possible-locations-for-sequence-picture-parameter-sets-for-h-264-stream/24890903#24890903



