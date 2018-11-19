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
MENU_SELE=1
MENU_ITEMS="返回"

if [ "x${USB_GADGET_ENABLED}" = "x1" ]; then
	MENU_ITEMS="${MENU_ITEMS} 关闭设备"
else
	MENU_ITEMS="${MENU_ITEMS} 启用设备"
fi

if [ "x${USB_GADGET_FUNCTION}" = "x0" ]; then
	[ "x${USB_GADGET_ENABLED}" = "x0" ] || MENU_SELE=3
	MENU_ITEMS="${MENU_ITEMS} 虚拟网卡(当前) 文件存储"
else
	[ "x${USB_GADGET_ENABLED}" = "x0" ] || MENU_SELE=4
	MENU_ITEMS="${MENU_ITEMS} 虚拟网卡 文件存储(当前)"
fi
}

check_env || { echo "Requires luci-app-usb_gadget & lbk-cmd !" && exit 1; }
load_config || { echo "Failed to get settings !" && exit 1; }

init_menu_items

lbk-menu --align center --long-press off --select ${MENU_SELE} ${MENU_ITEMS} > /dev/null 2>&1
case "$?" in
	0)
	lbk-message --k2 none --k3 none --type info --topic "提示" --timeout 1 "操作取消"
	exit 0
;;
	1)
	exit 0
;;
	2)
	if [ "x${USB_GADGET_ENABLED}" = "x1" ]; then
		uci set usb_gadget.@usb_gadget[0].enabled=0
	else
		uci set usb_gadget.@usb_gadget[0].enabled=1
	fi
;;
	3)
	uci set usb_gadget.@usb_gadget[0].type=0
;;
	4)
	uci set usb_gadget.@usb_gadget[0].type=1
;;
	*)
	exit 1
;;
esac

lbk-message --k2 none --k3 none --type info --topic "信息" --timeout 0 "重启服务中..."

uci commit usb_gadget

/etc/init.d/usb_gadget reload || { lbk-message --k2 none --k3 exit --type stop --topic "错误" "无法应用设置" && exit 1; }

lbk-message --k2 none --k3 none --type idea --topic "信息" --timeout 1 "操作成功"

exit 0

