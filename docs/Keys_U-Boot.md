# U-Boot 按键作用

所提供的固件中 U-Boot，在其启动时可依相应按键，分别采用不同的已定义策略。

* K1：将启动 OpenWrt 的串口调试信息输出（默认不输出），具体详见固件中 images/apart/files/openwrt-spi-boot.cmd。
* K2：将在 OLED 上显示 U-Boot 的调试信息，方便外接键盘操作或查看相关信息。
* K3：将忽略内置 SPI Nor Flash 启动流程，尝试搜索 USB 存储介质、网络等进行启动，具体定义详见上述 openwrt-spi-boot.cmd。
