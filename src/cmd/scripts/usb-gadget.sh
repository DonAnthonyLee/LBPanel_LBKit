#!/bin/sh

check_env() {
[ -e /etc/config/usb_gadget -a -x /etc/init.d/usb_gadget ] || return 1
[ -x /usr/bin/lbk-cmd ] || return 1 
return 0
}

load_config() {
[ -x /sbin/uci ] || return 1
USB_GADGET_ENABLED=`uci get usb_gadget.@usb_gadget[0].enabled`
USB_GADGET_FUNCTION=`uci get usb_gadget.@usb_gadget[0].type`
return 0
}

init_menu_items() {
MENU_ITEMS=

if [ "x${USB_GADGET_ENABLED}" = "x1" ]; then
	MENU_ITEMS="${MENU_ITEMS} 关闭设备"
else
	MENU_ITEMS="${MENU_ITEMS} 启用设备"
fi

if [ "x${USB_GADGET_FUNCTION}" = "x0" ]; then
	MENU_ITEMS="${MENU_ITEMS} 虚拟网卡(当前) 文件存储"
else
	MENU_ITEMS="${MENU_ITEMS} 虚拟网卡 文件存储(当前)"
fi
}

check_env || { echo "Depend on luci-app-usb_gadget & lbk-cmd !" && exit 1; }
load_config || { echo "Failed to get configuration !" && exit 1; }

init_menu_items

lbk-menu --align center ${MENU_ITEMS} > /dev/null 2>&1
case "$?" in
	1)
	if [ "x${USB_GADGET_ENABLED}" = "x1" ]; then
		uci set usb_gadget.@usb_gadget[0].enabled=0
	else
		uci set usb_gadget.@usb_gadget[0].enabled=1
	fi
;;

	2)
	uci set usb_gadget.@usb_gadget[0].type=0
;;

	3)
	uci set usb_gadget.@usb_gadget[0].type=1
;;

	*)
	exit 1
;;
esac

uci commit usb_gadget
/etc/init.d/usb_gadget reload || exit 1

exit 0

