# OpenMiko

OpenMiko is custom opensource firmware for cameras that use the Ingenic T20 chip.
These cameras include the Wyzecam V2 and Xiaomi Xiaofang.

The firmware aims to provide an alternative to the closed source out of box firmwares which can often be riddled with bugs and security holes. Privacy is also a concern as it is difficult to tell if out of box firmware reaches out to other servers or is broadcasting metadata.

## Features

- No app to download
- Support for common protocols such as RTSP and MJPEG
- SSH server
- ffmpeg
- mjpg-streamer
- Easily add or compile your own applications
- Supported by OctoPrint (https://community.octoprint.org/t/wyze-cam-v2-and-octoprint-as-of-28dec2020/27081)

## Differences between this project and DaFang Hacks / OpenFang

This project is focused on providing a better foundation for developers and end users.
At the time of this project OpenFang was relatively quiet. DaFang Hacks has some activity,
but I felt the way it was made was not necessarily conducive to developing on a solid platform.

This project generously uses the code from both projects, and it is much appreciated.

A few of the quality of life improvements in this project aimed at developers:

- A standardized toolchain based on [Buildroot](https://buildroot.org/)
- Docker image for development with precompiled artifacts available for download
- A compiled `uboot` based bootloader with USB ethernet and ext4. Load kernel images (`uImage.lzma`)
via TFTP for faster development.


## For end users

This firmware is a drop in replacement for the stock firmware. Care has been taken to ensure the firmware is compatible with the current flashing methods available. This means there is no need to rewrite the bootloader and that future firmware releases from the original manufacturer should continue to work.

For help join our Slack Channel:

https://join.slack.com/t/openmiko/shared_invite/zt-vcu9bpqt-VbEhxli_jHapEyj1Xu0ZAg

## Overview

At the present time, this repository only contains kernel and rootfs for cameras using Inegnic T20 SOC. To ease identifying these cameras please use the pictures below. A more detailed technical description can be found [here](doc/overview.md).

Ingenic T20X (128Mb DDR) | &nbsp;
:-- | --:
![Wyze Cam v2](doc/wyzecam_v2/img/wyzecam_v2.jpg) Wyze Cam V2 | 
![Xiaomi Dafang](doc/xiaomi_dafang/img/xiaomi_dafang.jpg) Xiaomi Dafang | ![Wyze Cam Pan](doc/WYZECP1/img/wyzecam_pan.jpg) Wyze Cam Pan

If you have a device with an Ingenic T10 SOC, consider using for now https://github.com/EliasKotlyar/Xiaomi-Dafang-Hacks

If you have a classic XiaoFang with an ARM-Processor, consider using https://github.com/samtap/fang-hacks

## Installation

Before installing OpenMiko, we suggest you carefully read the [FAQ](/doc/faq.md).

Download the specific version of the firmware you wish to install. The releases can be found on the right hand side in GitHub under the label "Releases".

[![Releases](doc/img/releases.png)](https://github.com/openmiko/openmiko/releases)

Rename the firmware to `demo.bin` and put it in on the root of the microSD card.

[![Assets](doc/img/assets.png)](https://github.com/openmiko/openmiko/releases)


Power off the camera and insert the microSD card.

Hold the setup button, plug in your USB cable, keep holding the setup button for 1-2 seconds until the light is solid blue, then release the button.

After about 30 seconds you should get a flashing yellow LED which indicates the camera is working.

## Configuration

### Generating config files

There are two BASH scripts that generate the `openmiko.conf` and `wpa_supplicant.conf`. These are found in the `utilities` directory:
- [openmiko-gen.sh](/utilities/openmiko-gen.sh)
- [wpa-gen.sh](/utilities/wpa-gen.sh)

### Copying config files

On your sdcard, create a directory `/config/overlay`. Any files added to this directory tree will be copied in place on to the camera's internal filesystem on boot.

Copy the generated `openmiko.conf` and `wpa_supplicant.conf` to `/config/overlay/etc/` on the sdcard for them to be copied to `/etc/` on boot.

![Overlay Filesystem](doc/img/overlay_filesystem.png)

Other files you may want to configure are:
- `/config/overlay/etc/passwd` and `/config/overlay/etc/shadow` can be overwritten to make sure password changes are persistent
- `/config/overlay/etc/dropbear/dropbear_ecdsa_host_key` can be used to have a persistent SSH signature
- `/config/overlay/etc/hostname` can be used to set the hostname
- `/config/overlay/etc/TZ` can be used to set the timezone
- `/config/overlay/etc/v4l2rtspserver.conf` can be used to configure the RTSP server
- `/config/overlay/etc/videocapture_settings.json` can be used to configure the video stream settings

### Manual configuration

#### Manual Openmiko configuration

On the sdcard create the directory `/config/overlay/etc`.

In the `etc` directory copy the file [`wpa_supplicant.conf`](/overlay_minimal/etc/openmiko.conf).

Openmiko has many settings that can be altered through the `openmiko.conf` file. See it's contents for details.

Pay particular attention to the variable `WIFI_MODULE`. Use 8189fs for WyzeCam V2. For the WyzeCam Pan and Dafang use 8189es.

Insert the sdcard into the camera and reboot. OpenMiko will copy this directory over to the `/config` partition (which is persistent flash storage). This method can also be used to overwrite other files. For example:

#### Manual wifi configuration

On the sdcard create the directory `/config/overlay/etc`.

In the `etc` directory copy the file [`wpa_supplicant.conf`](/overlay_minimal/etc/wpa_supplicant.conf).

Edit this file and plug in your Wi-Fi name and password.

Insert the sdcard into the camera and reboot. OpenMiko will copy this directory over to the `/config` partition (which is persistent flash storage). This method can also be used to overwrite other files. For example:

#### Manual video stream configuration

Settings can be changed by editing /etc/videocapture_settings.json. However, the changes will not persist unless you write them to the flash. To ease saving these settings copy the file to `/config/overlay/etc/videocapture_settings.json`. The files in /config are mounted to the flash chip and will survive reboots.

```
"general_settings": {
	"flip_vertical": 0,			// 1 flips image along vertical axis, 0 disables
	"flip_horizontal": 0,		// 1 flips image along horizontal axis, 0 disables
	"show_timestamp": 1 		// 1 enables timestamp, 0 disables
},
```
Please refer this file at https://github.com/openmiko/ingenic_videocap/blob/master/settings.json and make changes as necessary. It is suggested to have a copy of this file while you are installing the firmware.

### Resetting the configuration

- While the camera is started hold down reset button for at least 6 seconds.
- After 6 seconds the blue LED should turn on and pulse 3 heartbeats.
- The `/config` partition (which is mounted to the persistent flash memory itself) will be removed.

## Usage

The configuration by default provides 2 output streams:

- 1920x1080 H264
- 1920x1080 JPG over HTTP (MJPEG)

The streams can be accessed using the following URLs. Please make sure to replace 'Your_Camera_IP' with the correct IP of your camera:

- rtsp://Your_Camera_IP:8554/video3/unicast
- http://Your_Camera_IP:8080/?action=stream
- http://Your_Camera_IP:8080/?action=snapshot

## Accessing the camera via SSH

If you did not upload and `/etc/passwd` and `/etc/shadow` file to your camera, then it will start the SSH server with the default credentials.
- Username: root
- Password: root

You can SSH on to the camera and change these with the `passwd` command.

## Modules

Openmiko supports proprietary [modules](/doc/modules.md) for extending its functionality. It is recommended to use an `fsck` module that matches your sdcard's file system.

## Troubleshooting

If you can't seem to get up and running here are some things to check:

- Make sure you are using unix style line endings in wpa_supplicant.conf (this was fixed in a later release)
- Inside wpa_supplicant.conf the `psk` and `ssid` settings need to have double quotes around the string. For example `psk="password_keep_double_quotes"`
- Make sure the file does not have a `.txt` extension. The file should show up as `wpa_supplicant.conf`. `Not wpa_supplicant.conf.txt`
- The `config/overlay/etc` directory is *cAsE sEnsItiVe*. Make sure it is all lowercase.
- The MAC address does change when flashing from the one on the sticker. Check your router to see the new DHCP address.
- There are some reports that assigning a static IP / DHCP reservation does help the WyzeCam connect to the network
- Logs should appear on the sdcard if the system is properly booting
- Some WyzeCams have a buggy bootloader from the factory and won't flash anything. The only way around this is to flash a new bootloader.
- If you flashed the custom HD Dafang bootloader you will need to revert to the original one. A copy of the old WyzeCam V2 original bootloader is here: https://github.com/openmiko/openmiko/blob/master/stock_firmware/wyzecam_v2/wyzecam_v2_stock_bootloader.bin
- Join the Slack and ask a question for more help


## Issues and support

If you encounter an issue which you think is a bug in the OpenMiko or the associated libraries, you are welcome to submit it here on Github: https://github.com/openmiko/openmiko/issues.

Please provide as much context as possible:

- buildroot core version which you are using;
- kernel version and modules you have enable;
- build root packages you are trying to integrate;
- when encountering an issue which happens at run time, attach serial output. Wrap it into a code block, just like the code;
- for issues which happen at compile time, enable verbose compiler output in the buildroot preferences, and attach that output (also inside a code block);
- development board model and brand;
- other settings (board choice, flash size, etc.).


## Contributing

Pull requests are welcome. For fixes of code and documentation, please go ahead and submit a pull request. Information on setting up your development environment can be found [here](doc/development.md).
