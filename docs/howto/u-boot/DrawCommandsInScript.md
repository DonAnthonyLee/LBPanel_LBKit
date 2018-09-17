# 如何在启动脚本中绘制图标及提示信息

全志 H3/H5 主线 U-Boot 可支持扫描各个接口设备上的 boot 分区上的启动脚本 (文件名 boot.scr)　并按其执行，所以一般情况下，无需更改 U-Boot 的环境配置，而直接修改 boot.cmd 后，通过 mkimage 生成替换 boot.scr 即可。

先来看看 Ubuntu 加载时修改脚本内容
    --- boot.cmd.old
+++ boot.cmd
@@ -10,6 +10,8 @@
 setenv ramdisk_addr 0x47000000
 setenv dtb_addr 0x48000000
 
+if test -n "${oled_stage2_cmd}"; then run oled_stage2_cmd; fi;
+
 fatload mmc 0 ${kernel_addr} ${kernel}
 fatload mmc 0 ${ramdisk_addr} ${ramdisk}
 fatload mmc 0 ${dtb_addr} sun8i-${cpu}-${board}.dtb
@@ -27,4 +29,5 @@
 setenv fbcon map:0
 setenv bootargs console=ttyS0,115200 earlyprintk root=/dev/mmcblk0p2 rootfstype=ext4 rw rootwait fsck.repair=${fsck.repair} panic=10 ${extra} fbcon=${fbcon}
 
+if test -n "${oled_stage3_cmd}"; then run oled_stage3_cmd; fi;
 bootz ${kernel_addr} ${ramdisk_addr}:500000 ${dtb_addr}

可以见到，在相应指令前 run 我们需要执行的绘制命令即可。OLED 初始化后将设置 oled_stage2_cmd 和 oled_stage3_cmd，启动脚本可以直接使用，省去撰写冗长指令的麻烦。
 
举例，oled_stage2_cmd 环境内容如下：

    oled_update 0; \
oled_show 48 17 0 2 1; \
oled_clear 0 51 128 12 1; \
oled_show 10 51 12 提取内核...; \
oled_update 1;

第一个 oled_update 指令控制 OLED 暂停显示缓冲更新，参数为 0 表示直至再设定 update 前，绘图指令只操作缓冲区而不会更新到屏幕，防止加大闪烁。

上面第一个 oled_show 指令作用是将第 2 号图标（size=0时）按清除模式画到指定起始坐标（X=48, Y=17）处。

oled_clear 是清除指定矩形区域内容，可指定用加亮或者清除模式。

最后一个 oled_update 指令恢复显示更新，将影响区域的已绘制内容一次传输至屏幕上。
