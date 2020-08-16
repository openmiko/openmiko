# Installation



The installation of openfang is mainly composed of three steps. The first one consists on flashing a new bootloader capable to load all the ROOTFS directories entirely from the SD card partition, the second one consists on partitioning the SD cards into two partitions and writing the new ROOTFS in one of the partitions, and the third one consists on resizing the ROOTFS filesystem.

- [Download and flash the bootloader](#dowload-bootloader)
- [Download and write the rootfs image](#download-rootfs)
- [Resize the written filesystem](#resize-the-rootfs-image)

More specific install guides can be found in the following links
- [Wyze Pan Cam (WYZECP1)](/doc/WYZECP1/wyzecp1_instructions.md) (thanks to M@ SWARTZ)
- [Xiaomi Mijia V3 2018 (SXJ02ZM)](/doc/SXJ02ZM/SXJ02ZM_instructions.md) (thanks to TheRoss)

## 1. Dowload bootloader

:heavy_exclamation_mark:WARNING!! You may brick your device in this step. Be cautious.

- The bootloader for devices with 64M DDR can be found [here](https://github.com/anmaped/openfang/releases) (:heavy_exclamation_mark:WARNING!! >= RC6)
- The bootloader for devices with 128M DDR can be found [here](https://github.com/anmaped/openfang/releases)

### Flash the bootloader

This step assumes that you have got a ssh access to the device. More details on to get it [here](troubleshooting.md).

Please perform the following steps.
1) Download the file into the device using ssh.
```
scp u-boot-lzo-with-spl.bin root@<remote_host>:u-boot-lzo-with-spl.bin
```

2) Veryfy the SHA-256 checksum (98fc8fb0d74f0a65aa765b4bd03b15474911ba9b14709ef0d156a6a801048ea6)
```
sha256sum u-boot-lzo-with-spl.bin
```

3) Flash the firmware using dd command (always erase mtd0 device before flashing; otherwise you may break your device. Be cautious.)
```
flash_eraseall /dev/mtd0
dd if=<filename.bin> of=/dev/mtd0
```

## 2. Download rootfs

- ROOTFS for T20L (64Mb DDR) and T20X (128Mb DDR) devices [Link](https://github.com/anmaped/openfang/releases)

### Flash rootfs using windows 10

diskpart or disk management wizard can be used to make partitions.

<img src="/doc/img/use_diskpart.png" width="600">

To flash the image in the first partition of the sd card use the tool DiskImage 1.6 ([link](http://www.roadkil.net/program.php/P12/Disk%20Image)).

<img src="/doc/img/towrite.png" width="300">


### Flash rootfs using linux

```
fdisk /dev/sdb
```
Use `p` to see the list of partitions and `n` to create a new one.

After creating all the partitions we should see something like that

```
Command (m for help): p
Disk /dev/sdb: 58.4 GiB, 62730010624 bytes, 122519552 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x00000001

Device     Boot    Start       End   Sectors  Size Id Type
/dev/sdb1           2048  10242047  10240000  4.9G  7 HPFS/NTFS/exFAT
/dev/sdb2       10242048 122517503 112275456 53.6G  7 HPFS/NTFS/exFAT
```

The first partition is the boot partition and is where we should store the rootfs. For that use the command

```
dd if=/path/to/image/rootfs.ext2 of=/dev/sdb1
```
to flash the rootfs image.

### Flash rootfs using MacOS

Follow the same instructions for Linux.

## 3. Resize the rootfs image

This step should be performed after flashing the device through the ssh connection. Before starting this step ensure that you get a steady light from your device. It should start blinking and later on stop, which means you have got a successful initialization. More detail on troubleshooting [here](troubleshooting.md).

The rootfs image is smaller than the available partition where we have written the rootfs directories. To be able to use all the available space we allocated for the partition, we have to resize the filesystem using the command
```
resize2fs /dev/mmcblk0p1
```
It will take a short while. Note that the time it takes depends of the size of the partition you are resizing.

To format the second partition to exFat or Fat32, we can use the command

```
mkfs.exfat /dev/partition
```
to format the device as an exfat filesystem or
```
mkfs.fat -F 32 /dev/partition
```
to create a fat32 filesystem.
