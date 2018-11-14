#!/bin/sh

[ -x /bin/npi_hat_cmd -a -x /usr/bin/lbk-cmd -a -x /usr/bin/LBPanel ] || exit 1
[ -e /usr/share/scripts/lbpanel-uci-config.sh ] || exit 1
[ ! -z "$1" ] || exit 1

oled_update 0
oled_clear 0 0 128 16
oled_show 35 1 14 OpenWrt
oled_clear 0 16 128 34 0
oled_show 8 17 0 5 1
oled_show 48 17 0 6 1
oled_clear 0 51 128 12
oled_update 1

oled_show 88 17 0 7 1
oled_show 10 51 12 进入系统...

. /usr/share/scripts/lbpanel-uci-config.sh init $1
sleep 1
/usr/bin/LBPanel --conf $1 > /dev/null 2>&1 &


