################################################################################
#
# oss cmd
#
################################################################################

OSS_VERSION = v4.2-build2017
OSS_SOURCE = oss-$(OSS_VERSION)-src-gpl.tar.bz2
OSS_SITE = http://www.4front-tech.com/developer/sources/stable/gpl
#OSS_SITE_METHOD = git
#OSS_GIT_SUBMODULES = YES
OSS_DEPENDENCIES =
OSS_CONF_OPTS =

OSS_CONFIGURE_CMDS = mkdir $(@D)/build && cd $(@D)/build && ../configure --no-regparm --only-drv=

define OSS_BUILD_CMDS
	$(MAKE) CPLUSPLUS=$(TARGET_CXX) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D)/build/lib
	$(MAKE) CPLUSPLUS=$(TARGET_CXX) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D)/build/cmd
	$(MAKE) LIBTOOL="$(HOST_DIR)/usr/mipsel-ingenic-linux-uclibc/sysroot/usr/bin/libtool --tag=CC" CPLUSPLUS=$(TARGET_CXX) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D)/build/lib/libsalsa
endef

define OSS_INSTALL_TARGET_CMDS
	ln -s ossplay $(@D)/build/target/bin/ossrecord
	cp -a $(@D)/build/target/bin/* $(TARGET_DIR)/usr/bin
	cp $(@D)/build/target/lib/* $(TARGET_DIR)/usr/lib
	cp $(@D)/build/target/sbin/* $(TARGET_DIR)/usr/sbin
	cp -f $(@D)/build/lib/libsalsa/.libs/libsalsa.so.2.0.0 $(TARGET_DIR)/usr/lib/libasound.so.2.0.0
	cp $(@D)/build/lib/libOSSlib/libOSSlib.so $(TARGET_DIR)/usr/lib/libOSSlib.so
endef

$(eval $(generic-package))
