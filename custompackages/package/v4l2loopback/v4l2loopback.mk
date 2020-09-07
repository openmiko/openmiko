################################################################################
#
# v4l2loopback
#
################################################################################

V4L2LOOPBACK_VERSION = aba3067f81b343f4e80c588de895c1aeb0da4b76
V4L2LOOPBACK_SITE = $(call github,umlaeute,v4l2loopback,$(V4L2LOOPBACK_VERSION))
V4L2LOOPBACK_LICENSE = GPL-2.0+
V4L2LOOPBACK_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_V4L2LOOPBACK_UTILS),y)
define V4L2LOOPBACK_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/utils/v4l2loopback-ctl $(TARGET_DIR)/usr/bin/v4l2loopback-ctl
endef
endif

$(eval $(kernel-module))
$(eval $(generic-package))
