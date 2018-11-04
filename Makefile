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
  DEPENDS:=+etkxx-lite $(CXX_DEPENDS)
  TITLE:=LBKit - Little Board application Kit
  MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>
endef

define Package/lbk/description
  LBKit - Little Board application Kit
endef

define Package/lbk-dev
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=+etkxx-lite-dev +lite-beapi-dev $(CXX_DEPENDS)
  TITLE:=LBKit - Little Board application Kit
  MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>
endef

define Package/lbk-dev/description
  LBKit - Little Board application Kit
endef

define Package/lb_panel
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+etkxx-lite +lbk $(CXX_DEPENDS)
  TITLE:=LBPanel - Little Board Panel application
  MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>
endef

define Package/lb_panel/description
  LBPanel - Little Board Panel application
endef

CONFIGURE_ARGS+= \
	--prefix=/usr \
	--mandir=/usr/share/man \
	--infodir=/usr/share/info \
	--libdir=/usr/lib \
	--sysconfdir=/etc \
	--with-lite-beapi-config=$(STAGING_DIR)/usr/bin/lite-beapi-config \
	CXX=$(PKG_BUILD_DIR)/wrapper/g++-uc

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp -af ./src/* $(PKG_BUILD_DIR)/
	mkdir $(PKG_BUILD_DIR)/wrapper
	cp -f $(TOOLCHAIN_DIR)/bin/g++-uc $(PKG_BUILD_DIR)/wrapper/g++-uc
	cd $(PKG_BUILD_DIR)/wrapper && patch -p1 < $(CURDIR)/patches/g++-uc.patch
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
	$(INSTALL_DIR) $(1)/usr/lib/add-ons/lbk
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/add-ons/lbk/libnpi-oled-hat.so.0.0.0 $(1)/usr/lib/add-ons/lbk/npi-oled-hat.so
endef

define Package/lbk-dev/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liblbk.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/include/lbk
	$(CP) $(PKG_BUILD_DIR)/include/lbk/*.h $(1)/usr/include/lbk/
	$(INSTALL_DIR) $(1)/usr/include/lbk/add-ons
	$(CP) $(PKG_BUILD_DIR)/include/lbk/add-ons/*.h $(1)/usr/include/lbk/add-ons/
endef

define Package/lb_panel/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/LBPanel $(1)/usr/bin/LBPanel
endef

$(eval $(call BuildPackage,lbk))
$(eval $(call BuildPackage,lbk-dev))
$(eval $(call BuildPackage,lb_panel))
