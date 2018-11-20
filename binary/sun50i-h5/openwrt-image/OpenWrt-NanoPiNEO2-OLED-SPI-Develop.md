# 开发所用 Overlay 映像说明

将 OpenWrt-NanoPiNEO2-OLED-SPI-0.1.3-Develop-ext4.img.xz 解压后所得文件是 ext4fs 类型的分区映像，可挂载后复制文件到指定目标分区（以下假定目标分区为 /dev/sda3）。

        xz -d OpenWrt-NanoPiNEO2-OLED-SPI-0.1.3-Develop-ext4.img.xz
        mkdir -p /tmp/src
        mkdir -p /tmp/dst
        mount OpenWrt-NanoPiNEO2-OLED-SPI-0.1.3-Develop-ext4.img /tmp/src
        mount /dev/sda3 /tmp/dst
        cp -a /tmp/src/* /tmp/dst
        umount /tmp/src
        umount /tmp/dst

复制好文件后，用 LBPanel 的 “覆盖文件分区切换” 功能切换至目标分区，重新启动后即可用于编译了。

## 示例程序

登录 OpenWrt 管理页面，设置好 dropbear 后，通过 ssh 登录 OpenWrt，所处目录为 /root，其中各目录文件如下：

* include，包含 uClibc++ 头文件。
* debug，包含编译所需库文件（带编译信息），run-gdb.sh 可用于启动 gdb 调试程序（目前暂无法在线安装 gdb）。
* sample，简单示例程序 helloworld，输入 make 编译后目录下则生成可执行文件 helloworld。

