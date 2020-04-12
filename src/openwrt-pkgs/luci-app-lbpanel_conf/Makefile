include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI support for changing configuration of LBPanel
LUCI_DEPENDS:=+lbpanel +lbk-cmd +lbk-npi_oled_hat +luci-compat
LUCI_PKGARCH:=all

PKG_NAME:=luci-app-lbpanel_conf
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>

include $(TOPDIR)/feeds/luci/luci.mk

define Package/luci-app-lbpanel_conf/conffiles
/etc/config/lbpanel_conf
endef

define Package/luci-app-lbpanel_conf/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || /etc/init.d/lbpanel enable || true
endef

# call BuildPackage - OpenWrt buildroot signature
