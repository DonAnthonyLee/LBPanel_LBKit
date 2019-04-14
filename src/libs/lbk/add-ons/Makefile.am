AM_CFLAGS = -I../../../include -I../../include
AM_CXXFLAGS = -I../../../include -I../../include

if OPENWRT_BUILD
SUBDIRS = generic oled-ssd1306 lcd-st7735s
else
SUBDIRS = generic oled-ssd1306 lcd-st7735s vpd
endif

noinst_LTLIBRARIES = liblbk_add-ons.la

liblbk_add_ons_la_SOURCES =		\
	LBPanelDevice.cpp		\
	LBPanelBuffer.cpp		\
	LBPanelDeviceAddOn.cpp		\
	LBPanelCombiner.cpp		\
	LBPanelKeypad.cpp		\
	LBPanelTouchpad.cpp		\
	LBPanelScreen.cpp

DISTCLEANFILES = Makefile.in

