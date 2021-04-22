################################################################################
#
# v4l2cpp
#
################################################################################

# 5/24/2020 master Version
#V4L2CPP_VERSION = 2d162272eaace43c9b54787afdded03d40579e39
V4L2CPP_VERSION = c165469e94521b767b548c2b34dcbb57a027bb01
V4L2CPP_SITE = https://github.com/mpromonet/libv4l2cpp
V4L2CPP_SITE_METHOD = git
V4L2CPP_LICENSE = UNLICENSE
V4L2CPP_LICENSE_FILES = LICENSE
V4L2CPP_DEPENDENCIES = live555 log4cpp
V4L2CPP_CFLAGS = $(TARGET_CFLAGS)

ifeq ($(BR2_STATIC_LIBS),y)
V4L2CPP_CONFIG_TARGET = linux
V4L2CPP_LIBRARY_LINK = $(TARGET_AR) cr
else
V4L2CPP_CONFIG_TARGET = linux-with-shared-libraries
V4L2CPP_LIBRARY_LINK = $(TARGET_CC) -o
V4L2CPP_CFLAGS += -fPIC
endif

ifndef ($(BR2_ENABLE_LOCALE),y)
V4L2CPP_CFLAGS += -DLOCALE_NOT_USED
endif

define V4L2CPP_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" AR="$(TARGET_AR)" EXTRA_CXXFLAGS="$(V4L2CPP_CFLAGS)" -C $(@D) all
endef

define V4L2CPP_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) PREFIX="$(STAGING_DIR)/usr" -C $(@D) install
	$(INSTALL) -D -m 0755 $(@D)/libv4l2wrapper.so $(TARGET_DIR)/usr/lib
endef




$(eval $(generic-package))

