H264BITSTREAM_VERSION = 0.2.0
H264BITSTREAM_SITE = $(call github,aizvorski,h264bitstream,$(H264BITSTREAM_VERSION))
H264BITSTREAM_DEPENDENCIES =
H264BITSTREAM_CONF_OPTS =
H264BITSTREAM_AUTORECONF = YES

$(eval $(autotools-package))
