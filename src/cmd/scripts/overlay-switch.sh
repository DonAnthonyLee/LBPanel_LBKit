#!/bin/sh

ROOTFS_DATA_MOUNT_DIR="/tmp/rootfs_data_mount"
OVERLAY_SOURCE_MOUNT_DIR="/tmp/overlay_source"

check_env() {
[ -e /etc/config/fstab -a -x /sbin/block ] || return 1
[ -x /usr/bin/lbk-cmd ] || return 1 
return 0
}

detect_rootfs_data() {
ROOTFS_DATA_DEV=
# OpenWrt always use mtd at first if possible
FOUND=`cat /proc/mtd | grep "rootfs_data" | awk -F ' ' '{printf $1}'`
if [ -z "$FOUND" ]; then
	# try root device as default overlay
	FOUND=`cat /proc/cmdline | grep "root="`
	if [ ! -z "$FOUND" ]; then
		ROOTFS_DATA_DEV=`echo ${FOUND##*root=} | awk -F ' ' '{printf $1}'`
	fi
else
	ROOTFS_DATA_DEV=`echo /dev/${FOUND%%:} | sed 's/mtd/mtdblock/'`
fi
[ ! -z "${ROOTFS_DATA_DEV}" ] || return 1

CUR_OVERLAY_DEV=`df | grep " /overlay" | awk -F ' ' '{printf $1}'`
#[ ! -z "${CUR_OVERLAY_DEV}" ] || return 1

EXT4FS_BLOCKS=`block info | grep "TYPE=\"ext4\"" | awk -F ' ' '{printf $1}' | sed 's/:/ /g'`

#echo "ROOTFS_DATA_DEV=${ROOTFS_DATA_DEV}"
#echo "CUR_OVERLAY_DEV=${CUR_OVERLAY_DEV}"
#echo "EXT4FS_BLOCKS=${EXT4FS_BLOCKS}"

return 0
}

mount_rootfs_data() {
mkdir -p ${ROOTFS_DATA_MOUNT_DIR} > /dev/null 2>&1
if [ "x" = "x${CUR_OVERLAY_DEV}" ]; then
	mount --bind / ${ROOTFS_DATA_MOUNT_DIR} > /dev/null 2>&1
	UCI="uci -c ${ROOTFS_DATA_MOUNT_DIR}/etc/config"
	touch ${ROOTFS_DATA_MOUNT_DIR}/etc/config/fstab
elif [ "x${ROOTFS_DATA_DEV}" = "x${CUR_OVERLAY_DEV}" ]; then
	mount --bind /overlay ${ROOTFS_DATA_MOUNT_DIR} > /dev/null 2>&1
	touch ${ROOTFS_DATA_MOUNT_DIR}/upper/etc/config/fstab
else
	ROOTFS_DATA_FSTYPE=`block info ${ROOTFS_DATA_DEV} | awk -F 'TYPE="' '{printf $2}' | sed 's/"//'`
	if [ "x${ROOTFS_DATA_FSTYPE}" = "xjffs2" ]; then
		mount -t jffs2 -o rw ${ROOTFS_DATA_DEV} ${ROOTFS_DATA_MOUNT_DIR} > /dev/null 2>&1
		touch ${ROOTFS_DATA_MOUNT_DIR}/upper/etc/config/fstab
	else
		ROM_MOUNTED=`mount | grep "/rom" | grep "${ROOTFS_DATA_DEV}"`
		if [ ! -z "${ROM_MOUNTED}" ]; then
			mount -o rw,remount /rom > /dev/null 2>&1 && \
			mount --bind /rom ${ROOTFS_DATA_MOUNT_DIR} > /dev/null 2>&1
		else
			mount -t ${ROOTFS_DATA_FSTYPE} -o rw ${ROOTFS_DATA_DEV} ${ROOTFS_DATA_MOUNT_DIR} > /dev/null 2>&1
		fi
		UCI="uci -c ${ROOTFS_DATA_MOUNT_DIR}/etc/config"
		touch ${ROOTFS_DATA_MOUNT_DIR}/etc/config/fstab
	fi
fi
[ "$?" = "0" ] || return 1
return 0
}

umount_rootfs_data() {
sync;sync;sync
umount ${ROOTFS_DATA_MOUNT_DIR}
rmdir ${ROOTFS_DATA_MOUNT_DIR}
}

remove_extroot_uuid() {
[ "x$1" != "x" ] || return 1
[ "x${ROOTFS_DATA_DEV}" != "x$1" ] || return 0

mkdir -p ${OVERLAY_SOURCE_MOUNT_DIR} > /dev/null 2>&1

mount -t ext4 -o rw,sync $1 ${OVERLAY_SOURCE_MOUNT_DIR} > /dev/null 2>&1

[ "$?" = "0" ] || { rmdir ${OVERLAY_SOURCE_MOUNT_DIR} && return 1; }
[ ! -e ${OVERLAY_SOURCE_MOUNT_DIR}/etc/.extroot-uuid ] || rm -f ${OVERLAY_SOURCE_MOUNT_DIR}/etc/.extroot-uuid

umount ${OVERLAY_SOURCE_MOUNT_DIR}
rmdir ${OVERLAY_SOURCE_MOUNT_DIR}

return 0
}

find_item_dev() {
CUR_ITEM=3
SELE_ITEM_DEV=
for bo in ${EXT4FS_BLOCKS}; do
	if [ "x${ROOTFS_DATA_DEV}" = "x$bo" ]; then
		continue
	elif [ "x${CUR_ITEM}" = "x$1" ]; then
		SELE_ITEM_DEV="$bo"
		return 0
	fi
	CUR_ITEM=$((CUR_ITEM+1))
done
return 1
}

init_menu_items() {
MENU_SELE=1
MENU_ITEMS="返回"

if [ "x${ROOTFS_DATA_DEV}" = "x${CUR_OVERLAY_DEV}" -o "x${CUR_OVERLAY_DEV}" = "x" ]; then
	MENU_ITEMS="${MENU_ITEMS} 原始分区(+)"
	MENU_SELE=2
else
	MENU_ITEMS="${MENU_ITEMS} 原始分区"
fi

CUR_ITEM=3
for bo in ${EXT4FS_BLOCKS}; do
	if [ "x${ROOTFS_DATA_DEV}" = "x$bo" ]; then
		continue
	fi
	MENU_ITEMS="${MENU_ITEMS} $bo"
	if [ "x$bo" = "x${CUR_OVERLAY_DEV}" ]; then
		MENU_SELE=${CUR_ITEM}
		MENU_ITEMS="${MENU_ITEMS}(+)"
	fi
	CUR_ITEM=$((CUR_ITEM+1))
done

#echo "MENU_ITEMS=${MENU_ITEMS}"
#echo "MENU_SELE=${MENU_SELE}"
}

delete_old_overlay_settings() {
[ "x${ROOTFS_DATA_DEV}" != "x${CUR_OVERLAY_DEV}" ] || uci commit fstab
FOUND=`$UCI show fstab | grep ".target='/overlay'"`
if [ ! -z $FOUND ]; then
	FOUND="${FOUND%%.target=\'/overlay\'}"
	$UCI delete $FOUND > /dev/null 2>&1
fi
}

add_new_overlay_settings() {
	$UCI add fstab mount
	$UCI set fstab.@mount[-1].target=/overlay
	$UCI set fstab.@mount[-1].device=$1
	$UCI set fstab.@mount[-1].fstype=ext4
	$UCI set fstab.@mount[-1].options='rw,sync'
	$UCI set fstab.@mount[-1].enable_fsck=0
	$UCI set fstab.@mount[-1].enabled=1
}

check_env || { echo "Requires block-mount & lbk-cmd !" && exit 1; }

lbk-message --k2 none --k3 none --type info --topic "信息" --timeout 0 "正在检测..."

detect_rootfs_data || { \
lbk-message --k2 none --k3 exit --type stop --topic "错误" "检测失败!\\n(rootfs_data)" && exit 1; }

if [ -z "${EXT4FS_BLOCKS}" ]; then
	lbk-message --k2 none --k3 exit --type stop --topic "错误" "无法找到分区!\\n(ext4fs 类型)"
	exit 1
fi

mount_rootfs_data || { \
lbk-message --k2 none --k3 exit --type stop --topic "错误" "挂载分区失败!\\n(rootfs_data)" && exit 1; }

init_menu_items

lbk-menu --long-press off --select ${MENU_SELE} ${MENU_ITEMS} > /dev/null 2>&1
SELE_ITEM=$?
DST_OVERLAY_DEV=
case "${SELE_ITEM}" in
	0)
	umount_rootfs_data
	lbk-message --k2 none --k3 none --type info --topic "提示" --timeout 1 "操作取消"
	exit 0
;;
	1)
	umount_rootfs_data
	exit 0
;;
	2)
	if [ "x${ROOTFS_DATA_DEV}" != "x${CUR_OVERLAY_DEV}" ]; then
		DST_OVERLAY_DEV="${ROOTFS_DATA_DEV}"
	fi
;;
	*)
	find_item_dev ${SELE_ITEM}
	#echo "SELE_ITEM_DEV=$SELE_ITEM_DEV"
	if [ "x${SELE_ITEM_DEV}" != "x${CUR_OVERLAY_DEV}" ]; then
		DST_OVERLAY_DEV="${SELE_ITEM_DEV}"
	fi
;;
esac

if [ ! -z "${DST_OVERLAY_DEV}" ]; then
	#echo "DST_OVERLAY_DEV=${DST_OVERLAY_DEV}"

	lbk-message --k2 none --k3 none --type info --topic "信息" --timeout 0 "正在设置..."

	remove_extroot_uuid ${DST_OVERLAY_DEV}
	if [ $? != 0 ]; then
		umount_rootfs_data
		lbk-message --k2 none --k3 exit --type stop --topic "错误" "挂载分区失败!\\n(overlay)"
		exit 1
	fi

	delete_old_overlay_settings
	[ "${DST_OVERLAY_DEV}" == "${ROOTFS_DATA_DEV}" ] || \
		add_new_overlay_settings ${DST_OVERLAY_DEV}
	$UCI commit fstab

	sync;sync;sync
	lbk-message --type warning --topic "警告" "重启方可生效!\\n确定重启吗?"
	if [ "$?" = "2" ]; then
		reboot && lbk-message --k2 none --k3 none --type empty "正在重新启动..."
	fi
fi

umount_rootfs_data
lbk-message --k2 none --k3 none --type idea --topic "信息" --timeout 1 "操作成功"

exit 0

