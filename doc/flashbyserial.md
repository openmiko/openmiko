
## Flashing openfang bootloader using serial interface

#Note
Serial port's baud rate is 115200

Tera Term VT can be used as the tool to send the bootloader through YModem protocol.


```
loady # to use YModem protocol

sf erase 0x0 0x000000040000 # to erase the bootloader section
sf write 0x82000000 0x0 ${filesize} # to write the bootloader

reset # to load the new bootloader
```

