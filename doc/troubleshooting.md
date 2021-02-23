
# Troubleshooting

### I don't know how to connect my device to my wireless newtwork, how can I configure it?

You can use the following configuration:

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

Save this on the sdcard at `wpa_supplicant.conf`

Once you are in via SSH you can save it to  `/config/overlay/etc/wpa_supplicant.conf` and it will be persistent (without the sdcard).

### What is the root password ?

The default user/password is root/root.


### Errors in Logs

```
i264e[error]: invalidate input nal buffer align
i264e[error]: update_fenc failed
```
This happened when I set the encoders JSON buffer_size to 1.
Turns out the encoder's buffer size needs to 0. 


### Sensor debug logs

After starting the videocapture process you can find more info in the /proc filesystem:

```
# cat /proc/jz/isp/isp_info
****************** ISP INFO **********************
Software Version : H201712121018
Firmware Version : H01-380
SENSOR NAME : jxf23
SENSOR OUTPUT WIDTH : 1920
SENSOR OUTPUT HEIGHT : 1080
SENSOR OUTPUT RAW PATTERN : BGGR
SENSOR Integration Time : 998 lines
ISP Top Value : 0x137c2193
ISP Runing Mode : Day
ISP OUTPUT FPS : 25 / 1
SENSOR analog gain : 29
MAX SENSOR analog gain : 128
SENSOR digital gain : 0
MAX SENSOR digital gain : 0
ISP digital gain : 3
MAX ISP digital gain : 32
ISP gain log2 id : 1
ISP total gain : 512
ISP exposure log2 id: 718531
ISP AE strategy: AE_SPLIT_BALANCED
ISP Antiflicker : 0[0 means disable]
Evtolux Probability Enable  : 0
ISP WB Gain00 : 338
ISP WB Gain01 : 256
ISP WB Gain10 : 256
ISP WB Gain11 : 521
ISP WB rg : 247
ISP WB bg : 224
ISP WB Temperature : 3600
ISP Black level 00 : 65
ISP Black level 01 : 65
ISP Black level 10 : 65
ISP Black level 11 : 66
ISP LSC mesh module : enable
ISP LSC mesh blend mode : 2
ISP LSC mesh R table : 0
ISP LSC mesh G table : 0
ISP LSC mesh B table : 0
ISP LSC mesh R blend : 176
ISP LSC mesh G blend : 176
ISP LSC mesh B blend : 176
ISP LSC mesh shading strength : 1024
ISP Sinter thresh1h : 5
ISP Sinter thresh4h : 4
ISP Sinter thresh1v : 5
ISP Sinter thresh4v : 4
ISP Sinter thresh short : 12
ISP Sinter thresh long : 12
ISP Sinter strength1 : 255
ISP Sinter strength4 : 255
ISP Temper thresh module :enable
ISP Temper thresh short : 95
ISP Temper thresh long : 95
ISP Iridix strength : 69
ISP Defect pixel threshold : 64
ISP Defect pixel slope : 1350
ISP sharpening directional : 100
ISP sharpening undirectional : 115
ISP FR sharpen strength : 10
ISP DS1 sharpen strength : 50
ISP DS2 sharpen strength : 70
ISP CCM R_R : 494
ISP CCM R_G : 32937
ISP CCM R_B : 32837
ISP CCM G_R : 32888
ISP CCM G_B : 32882
ISP CCM G_G : 490
ISP CCM B_R : 32818
ISP CCM B_G : 33084
ISP CCM B_B : 622
Saturation Target : 128
Saturation : 128
Sharpness : 128
Contrast : 127
Brightness : 128
```

### Debugging with gdb

https://www.oreilly.com/library/view/mastering-embedded-linux/9781787283282/43e79c1d-f609-456d-841e-41c595245c5e.xhtml

Had to remove libiberty.a from `output/host`
http://lists.busybox.net/pipermail/buildroot/2015-July/131981.html


[Buildroot] Failed to build gdb on host side
Arnout Vandecappelle arnout at mind.be
Thu Jul 9 22:56:31 UTC 2015
Previous message: [Buildroot] Failed to build gdb on host side
Next message: [Buildroot] [PATCH v2 1/2] ltrace: update to current master
Messages sorted by: [ date ] [ thread ] [ subject ] [ author ]
On 07/07/15 11:44, Viallard Anthony wrote:
> On 07/03/2015 07:02 PM, Arnout Vandecappelle wrote:
>> On 07/03/15 12:29, Viallard Anthony wrote:
[snip]
> The file libiberty.a seems to come from the gcc-final package 

 Right, seems that gcc 4.7.4 installs it but later versions don't do that anymore.

 Simplest workaround is to remove libiberty.a from HOST_DIR after the build. I'm
not sure if it's still worth it to do this in buildroot itself.

 Regards,



If you are using Buildroot, you will find that the sysroot is in output/host/usr/<toolchain>/sysroot, and that there is a symbolic link to it in output/staging. So, for Buildroot, you would ...


set sysroot output/staging/


target remote 172.26.0.2:4444