include $(TOPDIR)/rules.mk

PKG_NAME:=LBPanel_LBKit
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=README

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/lbk
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+etkxx $(CXX_DEPENDS)
  TITLE:=LBKit - Little Board application Kit
  MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>
endef

define Package/lbk/description
  LBKit - Little Board application Kit
endef

define Package/lbk-cmd
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+lbk $(CXX_DEPENDS)
  TITLE:=Utilities for LBKit
  MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>
endef

define Package/lbk-cmd/description
  Utilities for LBKit
endef

define Package/lbk-dev
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=+lite-beapi-dev $(CXX_DEPENDS)
  TITLE:=LBKit - Little Board application Kit
  MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>
endef

define Package/lbk-dev/description
  LBKit - Little Board application Kit
endef

define Package/lbpanel
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+lbk +lbk-cmd $(CXX_DEPENDS) +block-mount +luci-app-usb_gadget
  TITLE:=LBPanel - Little Board Panel application
  MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>
endef

define Package/lbpanel/description
  LBPanel - Little Board Panel application
endef

define Package/lbk-npi_oled_hat
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+lbk $(CXX_DEPENDS) +kmod-oled_ssd1306
  TITLE:=Commands and library for NanoPi OLED hat using LBKit
  MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>
endef

define Package/lbk-npi_oled_hat/description
  Commands and library for NanoPi OLED hat using LBKit
endef

CONFIGURE_ARGS+= \
	--prefix=/usr \
	--mandir=/usr/share/man \
	--infodir=/usr/share/info \
	--libdir=/usr/lib \
	--datadir=/usr/share \
	--sysconfdir=/etc \
	--with-lite-beapi-config=$(STAGING_DIR)/usr/bin/lite-beapi-config

ifeq ($(CONFIG_USE_UCLIBCXX),y)
	CONFIGURE_ARGS+= \
		CXX=$(PKG_BUILD_DIR)/wrapper/g++-uc
endif

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp -af ./src/* $(PKG_BUILD_DIR)/
ifeq ($(CONFIG_USE_UCLIBCXX),y)
	mkdir $(PKG_BUILD_DIR)/wrapper
	cp -f $(TOOLCHAIN_DIR)/bin/g++-uc $(PKG_BUILD_DIR)/wrapper/g++-uc
	cd $(PKG_BUILD_DIR)/wrapper && patch -p1 < $(CURDIR)/patches/g++-uc.patch
endif
endef

define Build/Compile
	sed -i 's/-L \//-L\//g' $(PKG_BUILD_DIR)/config.status
	cd $(PKG_BUILD_DIR)/ && ./config.status
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblbk.so* $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/add-ons/lbk
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/add-ons/lbk/libnpi-oled-hat.so.0.0.0 $(STAGING_DIR)/usr/lib/add-ons/lbk/npi-oled-hat.so
	mkdir -p $(STAGING_DIR)/usr/include/lbk
	$(CP) $(PKG_BUILD_DIR)/include/lbk/*.h $(STAGING_DIR)/usr/include/lbk/
	mkdir -p $(STAGING_DIR)/usr/include/lbk/add-ons
	$(CP) $(PKG_BUILD_DIR)/include/lbk/add-ons/*.h $(STAGING_DIR)/usr/include/lbk/add-ons/
endef

define Build/UninstallDev
	rm -rf \
		$(STAGING_DIR)/usr/lib/liblbk.so* \
		$(STAGING_DIR)/usr/lib/add-ons/lbk \
		$(STAGING_DIR)/usr/include/lbk
endef

define Package/lbk/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblbk.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc
	touch $(1)/etc/LBK.conf
endef

define Package/lbk/conffiles
/etc/LBK.conf
endef

define Package/lbk-cmd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/lbk-cmd $(1)/usr/bin/lbk-cmd
	ln -sf ./lbk-cmd $(1)/usr/bin/lbk-notify
	ln -sf ./lbk-cmd $(1)/usr/bin/lbk-menu
	ln -sf ./lbk-cmd $(1)/usr/bin/lbk-message
endef

define Package/lbk-dev/install
	$(INSTALL_DIR) $(1)/usr/include/lbk
	$(CP) $(PKG_BUILD_DIR)/include/lbk/*.h $(1)/usr/include/lbk/
	$(INSTALL_DIR) $(1)/usr/include/lbk/add-ons
	$(CP) $(PKG_BUILD_DIR)/include/lbk/add-ons/*.h $(1)/usr/include/lbk/add-ons/
endef

define Package/lbpanel/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/LBPanel $(1)/usr/bin/LBPanel
	$(INSTALL_DIR) $(1)/usr/share/scripts
	$(CP) $(PKG_INSTALL_DIR)/usr/share/scripts/* $(1)/usr/share/scripts/
endef

define Package/lbk-npi_oled_hat/install
	$(INSTALL_DIR) $(1)/usr/lib/add-ons/lbk
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/add-ons/lbk/libnpi-oled-hat.so.0.0.0 $(1)/usr/lib/add-ons/lbk/npi-oled-hat.so
	$(INSTALL_DIR) $(1)/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/npi_hat_cmd $(1)/bin/npi_hat_cmd
	ln -sf ./npi_hat_cmd $(1)/bin/oled_power
	ln -sf ./npi_hat_cmd $(1)/bin/oled_update
	ln -sf ./npi_hat_cmd $(1)/bin/oled_clear
	ln -sf ./npi_hat_cmd $(1)/bin/oled_show
endef

define Package/lbk-npi_oled_hat/postinst
#!/bin/sh
FOUND=`grep "npi-oled-hat" $${IPKG_INSTROOT}/etc/LBK.conf`
if [ -z "$$FOUND" ]; then
	echo "PanelDeviceAddon=/usr/lib/add-ons/lbk/npi-oled-hat.so" >> $${IPKG_INSTROOT}/etc/LBK.conf
fi
endef

$(eval $(call BuildPackage,lbk))
$(eval $(call BuildPackage,lbk-cmd))
$(eval $(call BuildPackage,lbk-dev))
$(eval $(call BuildPackage,lbk-npi_oled_hat))
$(eval $(call BuildPackage,lbpanel))
