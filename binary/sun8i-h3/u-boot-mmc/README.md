# U-Boot mmc 版说明

## 功能添加

* SSD1306 OLED (128x64) 显示

内建 oled_show, oled_clear, oled_update 等指令。

* ST7735S RGB LCD 显示

内建 lcd_show, lcd_clear, lcd_update 等指令。

* 中文字库

初始化后将使用 0x4b060000~0x4b1fffff 区域内存（约1.7MB）作为存储解压后字库及数据缓冲区，该内存区域数据将保留至 Linux Kernel 的相关驱动加载后才释放，因此内核映像加载等需确保不影响该区域。

* 检测 OLED Hat 按键状态

初始化时通过 env 的 oled_hat_key 输出状态，可在 bootcmd 使用。按键 K1、K2、K3 分别对应第 0、1、2 位;比如 K1 和 K2 同时按下时为 0x03 (00000011b)。

备注：

仅按下 K2 键时将启动 U-Boot 的字符终端模式，以供查看启动信息或外接 USB 键盘操作。


## 烧写

读卡器上更新（注意不要弄错编号毁了你的硬盘！！！）

    # dd if=./u-boot-mmc.bin of=/dev/sdXXX bs=1024 seek=8

## Ubuntu等启动问题

注意：文件替换前请备份原文件！

假如启动过程中出现无法挂载 root 文件系统而重启，请在所编译的设备树源文件（扩展名为 dts）中加入以下内容，并编译成 dtb 代替 Micro SD 卡上 boot 分区相应文件。

    &mmc0 {
    	boot_device = <1>;
    };

另外一种方法是更改 bootarg (修改 boot.cmd 并转换替换 boot.scr)，将其中的 mmcblk0 改成 mmcblk1，另外，有必要时需要修改或注释掉 rootfs 分区下 /etc/fstab 有关 mmcblk0 相关的行。 

## 其它

彩色面板相关详细见 [RGBPanel](https://github.com/DonAnthonyLee/LBPanel_LBKit/tree/rgb-panel/docs/RGBPanel.md)，更多使用帮助详细见 [HOWTO](https://github.com/DonAnthonyLee/LBPanel_LBKit/tree/master/docs/howto/u-boot)

