## 烧写

读卡器上更新（注意不要弄错编号毁了你的硬盘！！！）

    # dd if=./u-boot-mmc.bin of=/dev/sdXXX bs=1024 seek=8

## Ubuntu等启动问题

假如启动过程中出现无法挂载 root 文件系统而重启，请在所编译
的设备树源文件（扩展名为 dts）中加入以下内容，并编译成 dtb
代替 Micro SD 卡上 boot 分区相应文件。

    &mmc0 {
    	boot_device = <1>;
    };

另外一种方法是更改 bootarg (修改 boot.cmd 并转换替换 boot.scr)，将其中的 mmcblk0 改成
 mmcblk1。
 
 ## 其它

参照 SPI 启动。

