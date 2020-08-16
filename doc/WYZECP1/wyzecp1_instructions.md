# openfang Installation to Wyze Pan Cam (WYZECP1) from Stock Wyze Firmware

### A.	Follow instructions to flash Dafang Hacks to your Wyze Pan Cam (also below)
NOTE: If you already have Dafang installed and running on your camera, you can skip to part B [openfang Installation]

	1. Bootloader
		1a: Put microSD card in your computers card reader
		1b: Remove all volumes from SD card and create new partition for 512 MB
			(a) Alternatively you can shrink current volume to 512 MB
			(b) This step may not be neccessary but recommended to increase success rate
		1c: Format 512 MB partition to FAT32
		1d: Download custom firmware (cwf-1.0.bin) and copy to the 512 MB formatted microSD partition
			(a) https://github.com/EliasKotlyar/Xiaomi-Dafang-Hacks/raw/master/hacks/cfw/wyzecam_pan/cfw-1.0.bin
			(b) This is for the Wyze Pan Cam only
			(c) If you have another camera you may need a different CWF file
		1e: Rename "cfw-1.0.bin" on SD card to "demo.bin"
		1f: Remove SD card and pop it in your Wyze Pan Cam
		1g: Flash bootloader from microSD:
			(a) Remove power from camera
			(b) Hold setup button 
			(c) Continue to hold the setup button while you plug power cable into camera
			(d) Cotinue holding setup for another 10 seconds
			(e) Release setup button and let the camera finish flashing
			(f) Flashing could take 2-3 minutes
	2. Confirm Successful Flash (1): Firmware [4.10.0.222]
		2a: Open the Wyze App
			(a)	Right now your camera should still work with the Wyze App, so lets check the firmware version of the camera you just flashed
		2b: Navigate to the camera you're flashing and open Camera settings
		2c: Open Camera Settings (top right gear icon)
		2d: Navigateto Device Info
		2e: Check "Installed firmware version"
			(a) Should be version: 4.10.0.222
			(b) If not then restart at step [1g: Flash bootloader from microSD]
	3. Confirm Successful Flash (2): Install Dafang firmware to microSD
		3a: Put microSD card in your computers card reader
		3b: Remove all volumes from SD card and format entire card to FAT32
		3c: Trasnfer "firmware_mod" files to SD card
			(a) This is for reference. You should be following install_cfw.md for more detail
			(b) https://github.com/EliasKotlyar/Xiaomi-Dafang-Hacks/blob/master/hacks/install_cfw.md
		3d: Edit config/wpa_supplicant.conf accordingly (and renaming accordingly)
		3e: Remove SD card and pop it in your Wyze Pan Cam
		3f: Cycle power to camera
		3g: If you get the solid blue led then all is good, proceed to WebUI
		3h: In your browser, navigate to https://IP or https://dafang
			(a) username: root
			(b) password: ismart12
			(c) You don't need to login since this is proof of concept only. If you get to this screen then you can proceed with openfang installation

## B. openfang Installation

	1. Update Bootloader
		NOTE: You will be using Dafang firmware_mod files to update the bootloader
		1a: Download new bootloader found in "images-0d1384c.tar.xz"
		1b: Extract "openfang-images" from "images-0d1384c.tar" inside "images-0d1384c.tar.xz"
		1c: Navigate to "openfang-images" and find new bootloader "u-boot-lzo-with-spl.bin"
		1d: Access camera via FTP
			(a) The camera should be powered on with Dafang "firmware_mod" microSD installed
		1e: Navigate to /system/sdcard/ and copy "u-boot-lzo-with-spl.bin" here
		1f: Verify checksum [9f432fd663892b9214ccecce40cc45801dbb7adc0bf0fec11f3ef3e232d9a000]
				> Execute ssh: sha256sum u-boot-lzo-with-spl.bin
		1g: Flash new bootloader
			(a) Erase mtd0 device before flashing to mitigate risk of bricking device
				> Execute ssh: flash_eraseall /dev/mtd0
			(b) Flash new bootloader via "dd"
				> Execute ssh: dd if=u-boot-lzo-with-spl.bin of=/dev/mtd0
			NOTE: You may get an error during this step. If you see "209289 bytes (204.4KB) copied" you can ignore it
			NOTE: Find below example (ignore "seconds" and "KB/s" as this will vary)
					# 408+1 records in
					# 408+1 records out
					# 209289 bytes (204.4KB) copied, 1.009322 seconds, 202.5KB/s
	2. Flash rootsfs and Partition mircoSD
		NOTE: Use Windows Disk Managemant and/or EaseUS Partition Master to format and create partitions on microSD
		NOTE: Use Roadkil's DiskImage 1.6 to write rootfs (http://www.roadkil.net/program.php/P12/Disk%20Image)
		2a: Create new volume
			(a) Size: 2048 (2 GB)
			(b) Format FS: EXT2
			(c) Name: rootfs
		2b: Create second new volume
			(a) Size: 28387 (30 GB)
			(b) Format FS: exFAT
			(c) Name: openfang
		2c: Write "rootfs.ext2" to the 2 GB EXT2 partition using Roadkil's DiskImage 1.6
		2d: Copy "wpa_supplicant.conf" to the 30 GB exFAT partition
			(a) Use configuration from troubleshooting.md #4
			(b) https://github.com/anmaped/openfang/blob/master/doc/troubleshooting.md
			(c) Make sure to edit "wpa_supplicant.conf" with your network details
		2e: Remove SD card and pop it into your Wyze Pan Cam
		NOTE: You will see a mixture of solid and flashing LEDs in both blue and orange, this is good
			(a) If you get the solid orange led then all is good, proceed to WebUI
			(a) In your browser, navigate to https://IP or https://openfang
				i. username: admin
				ii. password: admin
				iii. Change password in Manage > Settings > HTTP Password

