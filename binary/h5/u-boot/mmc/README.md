## 烧写

读卡器上更新（注意不要弄错编号毁了你的硬盘！！！）

    # dd if=./u-boot-mmc.bin of=/dev/sdXXX bs=1024 seek=8

## Ubuntu等启动问题

使用官方 4.x 核心时，启动过程中可能出现无法挂载 root 文件系统而重启，可尝试按 NanoPi NEO 的方法进行修改。

###注意：文件替换前请备份原文件！

## 其它

参照 NanoPi NEO 的 SPI 启动。

