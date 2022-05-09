################################################################################
#
# v4l2loopback
#
################################################################################

V4L2LOOPBACK_VERSION = a6d82287eb734588a11c33e7281671c80c9bf6d7
V4L2LOOPBACK_SITE = https://github.com/umlaeute/v4l2loopback.git
V4L2LOOPBACK_SITE_METHOD = git
V4L2LOOPBACK_LICENSE = GPL-2.0+
V4L2LOOPBACK_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_V4L2LOOPBACK_UTILS),y)
define V4L2LOOPBACK_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/utils/v4l2loopback-ctl $(TARGET_DIR)/usr/bin/v4l2loopback-ctl
endef
endif

$(eval $(kernel-module))
$(eval $(generic-package))
