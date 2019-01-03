AM_CFLAGS = -I../../../include -I../../include
AM_CXXFLAGS = -I../../../include -I../../include

if OPENWRT_BUILD
SUBDIRS = npi-oled-hat
else
SUBDIRS =		\
	npi-oled-hat	\
	vpd
endif

noinst_LTLIBRARIES = liblbk_add-ons.la

liblbk_add_ons_la_SOURCES =		\
	LBPanelDevice.cpp		\
	LBPanelBuffer.cpp		\
	LBPanelDeviceAddOn.cpp		\
	LBPanelKeypad.cpp		\
	LBPanelKeypadGeneric.cpp	\
	LBPanelTouchpad.cpp		\
	LBPanelScreen.cpp

DISTCLEANFILES = Makefile.in

