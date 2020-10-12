# Common Issues and Questions


### Does it contain an RTSP Server?
Yes. It utilizes v4l2rtspserver.

### Does the connect to the Xiaomi Servers?
No. It does not connect to anything.

### Does it remove the original firmware?
Yes. However one of the goals is to preserve compatibility incase you want to flash back to stock. Thus the bootloader is not modified. This may change in the future.

### Can I run the distribution without a microsd card?
Yes.

## What features does the distribution currently have?
- Full working RTSP with H264/MJPEG. Based on https://github.com/mpromonet/v4l2rtspserver
- SSH-Server(dropbear) with username: root password: root (use passwd to change it later)
