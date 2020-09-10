#!/bin/sh

[ -x /bin/oled_cmd -o -x /bin/lcd_cmd ] || exit 1
[ -x /usr/bin/lbk-cmd -a -x /usr/bin/LBPanel ] || exit 1
[ -e /usr/share/scripts/lbpanel-uci-config.sh ] || exit 1
[ ! -z "$1" ] || exit 1

[ ! -x /bin/oled_cmd -o ! -c /dev/oled-003c ] || {
	oled_update 0
	oled_clear 0 0 128 16
	oled_show 35 1 14 OpenWrt
	oled_clear 0 16 128 34 0
	oled_show 8 17 0 5 1
	oled_show 48 17 0 6 1
	oled_show 88 17 0 7 1
	oled_clear 0 51 128 12
	oled_show 10 51 12 进入系统...
	oled_update 1
}

[ ! -c /dev/spi-lcd0.1 -a ! -c /dev/spi-lcd1.0 ] || {
	[ ! -x /bin/lcd_cmd -o ] || {
		[ ! -c /dev/spi-lcd0.1 ] || {
			lcd_update -D /dev/spi-lcd0.1 0
			echo "OpenWrt" > /sys/bus/spi/drivers/lcd_st7735s_spi/spi0.1/show_stage_title
			echo 7 > /sys/bus/spi/drivers/lcd_st7735s_spi/spi0.1/show_stage_icon
			echo "进入系统..." > /sys/bus/spi/drivers/lcd_st7735s_spi/spi0.1/show_stage_status
			lcd_update -D /dev/spi-lcd0.1 1
		}
		[ ! -c /dev/spi-lcd1.0 ] || {
			lcd_update -D /dev/spi-lcd1.0 0
			echo "OpenWrt" > /sys/bus/spi/drivers/lcd_st7735s_spi/spi1.0/show_stage_title
			echo 7 > /sys/bus/spi/drivers/lcd_st7735s_spi/spi1.0/show_stage_icon
			echo "进入系统..." > /sys/bus/spi/drivers/lcd_st7735s_spi/spi1.0/show_stage_status
			lcd_update -D /dev/spi-lcd1.0 1
		}
	}
}

. /usr/share/scripts/lbpanel-uci-config.sh init $1
sleep 1
SHELL= /usr/bin/LBPanel --conf $1 > /dev/null 2>&1 &

