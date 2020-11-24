#!/bin/sh

check_env() {
[ -x /usr/bin/LBPanel -a -x /usr/bin/lbk-cmd ] || return 1
return 0
}

init_config() {
rm -f $1 || return 1
touch $1 || return 1
chmod 600 $1 || return 1

VALUE=`uci get lbpanel_conf.@main[0].screen_off_timeout`
[ -z "$VALUE" ] || echo "LBPanel::ScreenOffTimeout=$VALUE" >> $1

VALUE=`uci get lbpanel_conf.@main[0].hours`
[ -z "$VALUE" ] || echo "LBPanel::24Hours=$VALUE" >> $1

VALUE=`uci get lbpanel_conf.@main[0].show_seconds`
[ -z "$VALUE" ] || echo "LBPanel::ShowSeconds=$VALUE" >> $1

INDEX=0
while ( true ); do
	ITEM_TITLE=`uci get lbpanel_conf.@custom_menu[$INDEX].title`
	[ ! -z "${ITEM_TITLE}" ] || break

	ITEM_COMMAND=`uci get lbpanel_conf.@custom_menu[$INDEX].command`
	[ ! -z "${ITEM_COMMAND}" ] || break

	ITEM_ARGS=`uci get lbpanel_conf.@custom_menu[$INDEX].args`
	if [ -z "${ITEM_ARGS}" ]; then
		echo "LBPanel::MenuItem=${ITEM_TITLE},${ITEM_COMMAND}" >> $1
	else
		echo "LBPanel::MenuItem=${ITEM_TITLE},${ITEM_COMMAND},${ITEM_ARGS}" >> $1
	fi

	INDEX=$((INDEX+1))
done

return 0
}

check_env || { echo "Requires LBPanel & lbk-cmd !" && exit 1; }
[ ! -z "$2" ] || exit 1

if [ "x$1" = "xinit" ]; then
	init_config $2 || return 1
elif [ "x$1" = "xreload" ]; then
	init_config $2 || return 1
	lbk-notify LBPanel SETTINGS_UPDATED || return 1
	[ ! -x /bin/oled_cmd ] || oled_power 1
fi

return 0

