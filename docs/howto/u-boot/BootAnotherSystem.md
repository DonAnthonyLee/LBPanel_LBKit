# 如何通过按键判定启动第二系统

此前介绍绘图指令时提到启动脚本，那么，不同系统可分别制作各自的启动脚本，并用 mkimage 分别转换为专用格式。之后，主启动脚本类似以下撰写，通过判定 oled_hat_key 的值来启动不同的系统。

    # Recompile with:
# mkimage -C none -A arm -T script -d boot.cmd boot.scr

if test ${oled_hat_key} = 4 ; then setenv my_script openwrt-boot.scr ; else setenv my_script ubuntu-boot.scr ; fi ;
fatload mmc 0 ${scriptaddr} ${my_script}
source ${scriptaddr}


以上脚本判定若用户按下 K3 键，则执行 openwrt-boot.scr 脚本，否则执行 ubuntu-boot.scr 脚本。