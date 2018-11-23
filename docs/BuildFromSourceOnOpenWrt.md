# 直接在 OpenWrt 上编译本项目

适应平台的 OpenWrt 固件自 0.1.4 版更新声卡驱动及修正 U-Boot 后，固件基本稳定；除非有重大修正，否则很长一段时间内我都不会再更新固件了。若然 LBPanel，或者 LBKit 有个别改进，可以尝试自己在 OpenWrt 编译并进行文件替换。


## 编译环境准备

先按开发 Overlay 所用映像的说明制作所需的覆盖文件分区，再用 LBPanel 的 “覆盖文件分区切换” 进行切换并重启进入开发环境。

把 patches/build_LBPanel_LBKit_on_OpenWrt.patch 文件通过 sftp 或其它方式上传至 /tmp 目录，本项目 src 目录上传至 /root；然后通过 ssh 登录至 OpenWrt 并运行以下命令对 src 目录下各级 Makefile.alone 进行 patch。

	cd /root/src
	patch -p2 < /tmp/build_LBPanel_LBKit_on_OpenWrt.patch


## 编译本项目

按上述准备好后，通过 ssh 登录至 OpenWrt，运行以下指令进行编译。

	cd /root/src
	make -f Makefile.alone


备注：

如果 LBKit 源码最后修改时间在所用固件发布时间之后，可能意味着 LBKit 将与原固件的 liblbk.so 会产生冲突，编译前运行下列指令暂时删除系统的 liblbk.so。

	/etc/init.d/lbpanel stop
	rm -f /usr/lib/liblbk.so

假如编译完后需要恢复该文件，运行下列指令即可。

	rm -f /overlay/upper/usr/lib/liblbk.so


## 主要文件替换

1. /root/src/libs/lbk/liblbk.so 替换 /usr/lib/liblbk.so
+ /root/src/libs/lbk/add-ons/npi-oled-hat/npi-oled-hat.so 替换 /usr/lib/add-ons/lbk/npi-oled-hat.so
+ /root/src/app/panel/LBPanel 替换 /usr/bin/LBPanel


