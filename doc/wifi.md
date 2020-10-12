

My understanding of what drivers are used to power the wireless in various camera models.


# DaFang uses:
insmod /driver/8189es.ko

# XiaoFang T20 uses:
insmod /driver/8189fs.ko

# WyzeCam v2 uses:
insmod /driver/rtl8189ftv.ko


However OpenMiko only uses 8189fs.ko and I have been testing on a WyzeCam V2.
