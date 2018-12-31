# 已知问题

* SPI0 总线接超过两个以上的设备时，无法启动

原因：U-Boot 的 SPI 驱动存在问题。

解决方法：使用 0.1.5 或再高版本固件。

<br>

* LBPanel 偶尔会出现按键无响应至假死状态（特别是系统负载较高时）。

原因：系统负载较高且或同时有多个按键事件时，依靠中断判别的 gpio_keys 驱动有时未能捕获相应状态变化。

解决方法：

1. LBKit 最新源码已针对该情况进行适当考虑及调整，可使用 0.1.3 或再高版本固件。

2. 执行以下指令重启 LBPanel

    /etc/init.d/lbpanel restart

<br>

* 执行 "关闭机器" 不能彻底关机，只会重新启动。

原因：看门狗（watchdog）关机过程依然使能。

结合 sun8i-h3 平台的情况来看，两者所采用看门狗驱动完全相同，而偏偏 sun50i-h5 则出现此问题，其根本原因初步估计是有个别 kmod 在 xxx_exit()/xxx_remove() 上出问题。

解决方法：暂时可使用 0.1.3 或再高版本固件解决这个问题。

暂时解决方法为看门狗的 reboot_notify_callback 函数加上 SYS_POWER_OFF 条件，即不再等待至 SYS_DOWN。

	--- a/drivers/watchdog/watchdog_core.c	2018-11-20 14:38:29.942277239 +0800
	+++ b/drivers/watchdog/watchdog_core.c	2018-11-20 14:38:44.222085401 +0800
	@@ -143,7 +143,7 @@
	 	struct watchdog_device *wdd = container_of(nb, struct watchdog_device,
	 						   reboot_nb);
	 
	-	if (code == SYS_DOWN || code == SYS_HALT) {
	+	if (code == SYS_POWER_OFF || code == SYS_DOWN || code == SYS_HALT) {
	 		if (watchdog_active(wdd)) {
	 			int ret;


