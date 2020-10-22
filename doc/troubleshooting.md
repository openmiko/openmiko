
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
