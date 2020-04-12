include $(TOPDIR)/rules.mk

LUCI_TITLE:=LuCI support for USB gadget
LUCI_DEPENDS:=+@USB_GADGET_SUPPORT +kmod-usb-gadget +kmod-usb-gadget-eth +kmod-usb-gadget-mass-storage +luci-compat
LUCI_PKGARCH:=all

PKG_NAME:=luci-app-usb_gadget
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_MAINTAINER:=Anthony Lee <don.anthony.lee@gmail.com>

include $(TOPDIR)/feeds/luci/luci.mk

define Package/luci-app-usb_gadget/conffiles
/etc/config/usb_gadget
endef

define Package/luci-app-usb_gadget/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || /etc/init.d/usb_gadget enable || true
endef

# call BuildPackage - OpenWrt buildroot signature
