################################################################################
#
# micropython-lib
#
################################################################################

MICROPYTHON_LIB_VERSION = 2829d4adc968a7208d96fa762a9ab4c1b744f73d # 1.9.3
MICROPYTHON_LIB_SITE = https://github.com/micropython/micropython-lib.git
MICROPYTHON_LIB_SITE_METHOD = git
MICROPYTHON_LIB_LICENSE = Python-2.0 (some modules), MIT (everything else)
MICROPYTHON_LIB_LICENSE_FILES = LICENSE

define MICROPYTHON_LIB_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) \
		PREFIX=$(TARGET_DIR)/usr/lib/micropython \
		install
endef

$(eval $(generic-package))
