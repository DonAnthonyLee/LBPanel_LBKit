## 需添加配件

SPI Nor Flash（考虑后期将储存 OpenWrt 的 rootfs，建议不小于8M）

## 功能添加

+ SPI 启动

适用：支持 Jedec 指令的 SPI Nor Flash （比如 Winbond, Atmel, Spansion 等）。

注意：启动时 SD 卡槽不要放置能支持启动的卡。

+ SSD1306 OLED (128x64) 显示

内建 oled_show, oled_clear, oled_update 等指令。

### 注意：

初始化后将使用 0x4b060000~0x4b1fffff 区域内存（约1.7MB）作为存储解压后字库及数据缓冲区，该内存区域数据将保留至 Linux Kernel 的 OLED 驱动加载后才释放，因此内核映像加载等需确保不影响该区域。

+ 字库

所提供 u-boot-spi.bin 文件自 512K 后为压缩字库，含一套 12 点阵 Unicode 字库、4 套 ASCII 字库（14/16/24/32）、一套图标（共 10 个 32x32 图标）。

+ 检测 OLED Hat 按键状态

初始化时通过 env 的 oled_hat_key 输出状态，可在 bootcmd 使用。按键 K1、K2、K3 分别对应第 0、1、2 位;比如 K1 和 K2 同时按下时为 0x03 (00000011b)。

备注：仅按下 K2 键时将启动 U-Boot 的字符终端模式，以供查看启动信息或外接 USB 键盘操作。

## 烧写及更新

+ 通过 FEL 方式

卸除 SD 卡槽的启动卡，用 OTG 线连接电脑即自动进入 FEL 模式，具体擦写指令请参考官方文档。

+ 直接用编程器烧写 u-boot-spi.bin 文件。

+ 通过 Linux 指令烧写或更新（以下假定 SPI Flash 对应 mtd0）

擦除 0~1024kB

        # flash_erase /dev/mtd0 0 256

写至 Flash

        # dd if=./u-boot-spi.bin of=/dev/mtd0

+ 通过 U-Boot 命令行更新，具体指令见相关资料。

提示：外接 USB 键盘并于开机时按住 K2 键及键盘上的 CTRL+C 可进入 U-Boot 命令行模式。

## 其它

更多使用帮助详细见 [HOWTO](https://github.com/DonAnthonyLee/LBPanel_LBKit/tree/master/docs/howto/u-boot)

