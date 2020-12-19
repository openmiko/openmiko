# Installation

Download the specific version of the firmware you wish to install.
Rename the firmware to `demo.bin` and put it in on the root of the microSD card.


Power off the camera and insert the microSD card.


Hold the setup button, plug in your USB cable, keep holding the setup button for 1-2 seconds until the light is solid blue, then release the button.

After about 30 seconds you should get a flashing yellow LED which indicates the camera is working.


## Flashing a version manually

Flashing a new version requires writing to the kernel and root flash memory regions.

Given the following partition table:

```
# cat /proc/mtd 
dev:    size   erasesize  name
mtd0: 00040000 00008000 "boot"
mtd1: 00200000 00008000 "kernel"
mtd2: 00d30000 00008000 "root"
mtd3: 00040000 00008000 "config"
mtd4: 00040000 00008000 "para"
mtd5: 00010000 00008000 "flag"
```

We are primarily concerned with writing to mtd1 and mtd2 using the OpenMiko firmware binary.

1. Copy the firmware binary to the camera. The binary is around 11MB (may be a tight fit)
2. Execute the following commands:

```
dd if=openmiko_firmware.bin skip=1 bs=64 | dd bs=512 count=$((0x200000/512)) of=/dev/mtdblock1
dd if=openmiko_firmware.bin skip=1 bs=$((64+0x200000)) | dd bs=512 count=$((0x890000/512)) of=/dev/mtdblock2
```

## Explanation


### Write kernel skipping first 64 bytes due to uImage header

Test command:
dd if=openmiko_firmware.bin skip=1 bs=64 | dd bs=512 count=$((0x200000/512)) | hexdump -C

Write command:
```
dd if=openmiko_firmware.bin skip=1 bs=64 | dd bs=512 count=$((0x200000/512)) of=/dev/mtdblock1
4096+0 records in
4096+0 records out
```

### Write root filesystem

Root file system is 8978432 bytes. In hex this is 0x890000. The mtd partition for the root filesystem is actually larger than this.


Test command (notice the first few bytes and rootfs.tar.xz). We skip the first 64 bytes (uImage header) and kernel (0x200000 bytes).
```
# dd if=openmiko_firmware.bin skip=1 bs=$((64+0x200000)) | dd bs=512 count=$((0x890000/512)) | hexdump -C -n 100

00000000  85 19 03 20 0c 00 00 00  b1 b0 1e e4 85 19 01 e0  |... ............|
00000010  35 00 00 00 4a 4e e0 cd  01 00 00 00 00 00 00 00  |5...JN..........|
00000020  02 00 00 00 17 a9 dd 5f  0d 08 00 00 d8 d5 8b c5  |......._........|
00000030  3d 28 f6 be 72 6f 6f 74  66 73 2e 74 61 72 2e 78  |=(..rootfs.tar.x|
00000040  7a ff ff ff 85 19 02 e0  44 10 00 00 6d 58 d1 84  |z.......D...mX..|
00000050  02 00 00 00 01 00 00 00  a4 81 00 00 00 00 00 00  |................|
00000060  0c 94 7d 00                                       |..}.|
00000064
```

### Write command
```
dd if=openmiko_firmware.bin skip=1 bs=$((64+0x200000)) | dd bs=512 count=$((0x890000/512)) of=/dev/mtdblock2
```
