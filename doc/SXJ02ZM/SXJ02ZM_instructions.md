# Introduction and requirements
<img src="/doc/SXJ02ZM/img/xiaomi_mijia_2018.jpg" width="300">

The Xiaomi Mijia 1080p v3 model is the snow-white model that was released in 2018, and has an Ingenic T20L SOC with 64MB of RAM.
Unfortunately there is no known firmware exploit that allows us to easily edit the firmware to get ssh access to the camera.
In order to install OpenFang on the camera, we need to program a custom bootloader for the camera using a ch341a programmer.



## Requirements
You will need:
- USB CH341A programmer ([amazon link](https://www.amazon.de/Programmer-CH341A-Burner-EEPROM-Writer/dp/B01D4CXYJE)) and Programmer Testing Clip for SOP8 packages ([amazon link](https://www.amazon.de/dp/B07GLB1M75/ref=cm_sw_r_tw_dp_U_x_aQdKCbTGZ0032))
<img src="/doc/SXJ02ZM/img/c341a_clamp.jpg" width="400">

- Soldering iron (any below 15w; you should avoid warming to much the SMD components)
- Solder Flux (any rosin flux)
- Phillips Screwdrivers (size PH00, or PH0)
- a piece of plastic foil (e.g. kapton tape) to isolate the vcc pin when programming the chip



# 1. Case disassembly
- Remove the back of the camera, starting from the botton.
<img src="/doc/SXJ02ZM/img/opencase.jpg" width="300">



- After the back has been losen, carefully remove the speaker/mic connector before removing the back.
<img src="/doc/SXJ02ZM/img/speakercable.jpg" width="300">



- Unscrew the 3 black screws to separate the board from the front cover. Leave the middle screws in there!
<img src="/doc/SXJ02ZM/img/screws.jpg" width="300">



- After removing the screws, carefully remove the 2 frontpanel LED connectors.
<img src="/doc/SXJ02ZM/img/frontpanelcables.jpg" width="300">




# 2. (De)soldering and preparing to flash
## Desoldering
- Desolder the VCC leg (Leg 8) of the SOP8 chip.
<img src="/doc/SXJ02ZM/img/W25Q128JVSQ.jpg" width="300">
<img src="/doc/SXJ02ZM/img/desolderedleg.jpg" width="300">




- Put a piece of plastic foil between the desoldered leg and the PCB. We do this to avoid powering-up the whole PCB through the chip+programmer, as the chip would net be flashable like this.
<img src="/doc/SXJ02ZM/img/foilundersop.jpg" width="300">



## Conecting the SPI Flash to the programmer
- Put the programmer clip tight on the SOP8 package, having in mind that the red cable is always for PIN1. Make sure it sits tight on the SOP8 package.
<img src="/doc/SXJ02ZM/img/clamponsop.jpg" width="300">
<img src="/doc/SXJ02ZM/img/clamponsop2.jpg" width="300">




- Connect the clip cable to the programmer as seen in here.
<img src="/doc/SXJ02ZM/img/c341a_position.jpg" width="300">


- Plug in the programer to the computer and start the flashing process.



# 3. Flashing the SPI Flash (Windows / Mac / Linux)
## Downloading the needed files
- Download the latest release package from [OpenFang/releases](https://github.com/anmaped/openfang/releases) and extract it somewhere.
- Download the proper flashing software from the tools directory. It is recommended to flash the SPI Flash under Linux / MacOS as I never had success in flashing it with windows! Feel free to report otherwise.
- You might have to recompile CH341aprog. Download the latest release at [setarcos/ch341prog](https://github.com/setarcos/ch341prog)and compile it.


### Flashing under MacOS / Linux
- Use ch341prog to first erase the SPI Flash: ``` ./ch341prog -e ```
- Use ch341prog to write the custom bootloader to the SPI Flash: ``` ./ch341prog -w u-boot-lzo-with-spl_t20_64M.bin ```  Caution: flash the 64M binary file!



### Flashing under Windows
- Download the [CH341A programmer v1.29](https://www.bios-mods.com/forum/Thread-CH341A-v1-29), extract it somewhere and run it.

- After clicking on connect, you will be prompted to select your flash memory. Select any of the two. 
<img src="/doc/SXJ02ZM/img/windows_flasher_2.png" width="300">

### Making backup
Attention: it's highly recommended NOT TO IGNORE THIS STEP, because this is the only way you can revert your camera's firmware back to stock!

- Click on "Read".

- When reading is completed, click on "Verify" to verify that reading result matches content on flash.

- Click on "Save" and save your backup.

### Flashing
- Click on erase at the top icons and wait for the process to finish. 
<img src="/doc/SXJ02ZM/img/windows_flasher_3.png" width="300">


- If the erasing has been completed, you will be prompted with a message. 
<img src="/doc/SXJ02ZM/img/windows_flasher_4.png" width="300">


- Click on read at the top icons and make sure that everything is set to FF. 
<img src="/doc/SXJ02ZM/img/windows_flasher_5.png" width="300">


- Click on File -> open and select the u-boot-lzo-with-spl_t20_64M.bin file. Do not select the 128m one. 
<img src="/doc/SXJ02ZM/img/windows_flasher_6.png" width="300">


- After the flashing, click on read at the top icons and see is something was written to the flash. 
<img src="/doc/SXJ02ZM/img/windows_flasher_7.png" width="300">



# 4. Preparing the SD-Card (Windows / Mac / Linux)
## Flash rootfs using Windows
- Download and install any partitioning software. My favourite freeware for this is Active Partition Manager.
- On active partiton manager erase all partitions of the sd-card.
- Create one NTFS-Partition with about 4GB (4096 mbytes)
- Select edit params of the partition and make sure you assign a drive letter to it.
- Use the rest of the unallocated space to create one exFat partition.
- Select edit params of the partition and make sure you assign a drive letter to it.
- Open DiskImage 1.6 ([link](http://www.roadkil.net/program.php/P12/Disk%20Image)) and flash the rootfs.ext2 imagefile onto the 4GB NTFS partition you created. <img src="/doc/img/towrite.png" width="300">



## Flash rootfs using linux
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



## Flash rootfs using MacOS
Follow the same instructions for Linux.



# 5. Configuration of the camera
If everything went well so far, your cam's front LED should be flashing in different colors, and after some couple of seconds should stay orange. If this is the case, you can now connect your computer to the temporary created hotspot from the camera for the initial setup.
When you connect to the OpenFang access point, you can open a browser and call https://192.168.14.1 in the browser to access the panel. Use admin / admin in order to log on.



## Resize the rootfs image
The rootfs image is smaller than the available partition where we have written the rootfs directories. To be able to use all the available space we allocated for the partition, we have to resize the filesystem.

- Log on a SSH shell to 192.168.14.1 with username admin / admin.
- On the shell type ``` su ``` press return.
- Type ``` resize2fs /dev/mmcblk0p1 ``` press return.
- It will take a short while. Note that the time it takes depends of the size of the partition you are resizing.



## Configure the camera on the WebUi
- Go to Settings and select the Model of the cam (in this case the Mijia 2018)
- Go to wireless in the settings, select the mode and insert your home router's network credentials. Don't chose the type AP, as it will create an acess point/hotspot and this is not what we want.

