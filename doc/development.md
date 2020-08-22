# Development

Run `docker-compose up -d` to automatically pull down a container with buildroot.

The container image is about 8GB. The reason for this is because the artifacts are all baked in.
This reduces the amount of time needed to get up and running.








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


