
# Troubleshooting

### 1. My led is blinking, what does that mean?

The led codes are:

    * orange led blinking: not yet initialized

    * orange led solid: successfully initialized

    * blue led: microsd card activity

### 2. I don't know how to get ssh access before flashing, what can I do?

You have to upload a modified bootloader using a fw hack or ask xiaomi/wyzecam for the ssh credential (xiaomi allows it but I'm not sure about wyzecam).

### 3. I have bricked my device, how can I recover it?

[TODO]

### 4. I don't know how to connect my device to my wireless newtwork, how can I configure it?

You can just use the following configuration.
```
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
ap_scan=1

network={
        ssid="SSID"
        # Uncomment to connect to Hidden SSIDs
        #scan_ssid=1 
        key_mgmt=WPA-PSK
        pairwise=CCMP TKIP
        group=CCMP TKIP WEP104 WEP40
        psk="PASSWORD"
        priority=2
}
```

You should do two partitions in the sdcard. One for the rootfs (ext2/ext3) and the second one in exfat to save the `wpa_supplicant.conf` file and other files of interest. It will connect to the wireless network according to the config instructions.

You only have to format the second partition in exfat. I didn't add support for fat32 due to the lack of wear-leveling.

### 5. What is the root password in case I want to switch to another build ?

Just use `su` and set your password using `passwd` tool.

The deafult user/password is admin/admin.


### 6. [Dafang-Hacks](https://github.com/Dafang-Hacks/rootfs) seems like a very similar effort to [Openfang](https://github.com/anmaped/openfang/). Which one should I choose to flash and contribute to?

Openfang is more flexible towards future/other devices and (currently) the most active, although being based on some original efforts made with the Dafang-hacks.

We are not disjoint forces since we are sharing information between both projects, but I will try to clarify some points. For some cases you just can choose Dafang-Hacks but if you are interested in OpenCV and on a platform that you want to modify then you should choose Openfang.

As one of the Dafang-hacks contributors said in October 2018:

> If you have flashed the custom uboot from Dafang-Hacks, you can also try the new rootfs (my pull request) or openfang rootfs directly. No need to flash the rom...
https://github.com/EliasKotlyar/Xiaomi-Dafang-Hacks/issues/573#issuecomment-427738793

Or yet:
> @arkhub you may want to try my pull request [Dafang-Hacks/rootfs#3](https://github.com/Dafang-Hacks/rootfs/pull/3) most stuff should work. Or you can have a look at https://github.com/anmaped/openfang promising but not yet fully functional.
https://github.com/EliasKotlyar/Xiaomi-Dafang-Hacks/issues/575#issuecomment-435716931

This means that both are interchangeble somehow. As a reminder, just now openfang has support for Xiaomi Mijia 2018 and Dafang-Hacks doesn't support it yet.

Openfang is a bit different from Dafang-hacks. Dafang Hacks as the name says is a hack and Openfang is built on top of Buildroot (it is a kernel, a bootloader, and a toolchain), which allows you to install and modify many other packages that you are not able to do with Dafang-Hacks (like using ubuntu or fedora on embedded systems), and Openfang is a complete opensource suite.

Buildroot is much wider and general tool for extending and developing new features and you could include what you want by just changing the configuration files, create a new firmware, adjust many other parameters in a more efficient and easy way.

In terms of packages, Openfang just uses the modified v4l2rtspserver provided from Dafang-Hacks but some changes are planned to be applied soon.

To sum-up Openfang is not a fork of Dafang-Hacks is more like a normal embeeded and extensible paltform for devices (e.g., cameras) using Ingenic SOCs that just born because of the limitations of a hack oriented development project like Dafang-Hacks.
