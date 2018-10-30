include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI support for USB OTG
LUCI_DEPENDS:=+kmod-usb-gadget +kmod-usb-gadget-eth +kmod-usb-gadget-mass-storage
LUCI_PKGARCH:=all

PKG_NAME:=luci-app-usb_otg
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>

include $(TOPDIR)/feeds/luci/luci.mk

define Package/luci-app-usb_otg/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || /etc/init.d/usb_otg enable || true
endef

# call BuildPackage - OpenWrt buildroot signature
