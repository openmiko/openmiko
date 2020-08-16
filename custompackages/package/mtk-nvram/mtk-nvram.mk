MTK_NVRAM_VERSION:= 0.0.1
MTK_NVRAM_SOURCE:= mtk-nvram-$(MTK_NVRAM_VERSION).tar.xz
MTK_NVRAM_SITE:= $(PWD)/package/mtk-nvram
MTK_NVRAM_SITE_METHOD:=file
MTK_NVRAM_INSTALL_TARGET:=YES

define MTK_NVRAM_BUILD_CMDS
        $(MAKE) CONFIG_NVRAM_MTD_NAME="$(BR2_PACKAGE_MTK_NVRAM_MENU)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define MTK_NVRAM_INSTALL_TARGET_CMDS
        $(INSTALL) -D -m 0755 $(@D)/flash $(TARGET_DIR)/usr/sbin
	$(INSTALL) -D -m 0755 $(@D)/nvram $(TARGET_DIR)/usr/sbin
	$(INSTALL) -D -m 0755 $(@D)/*.so $(TARGET_DIR)/usr/lib
endef

$(eval $(generic-package))

