AM_CFLAGS = -I../../include -I../include
AM_CXXFLAGS = -I../../include -I../include

SUBDIRS = add-ons

lib_LTLIBRARIES = liblbk.la

liblbk_la_SOURCES =			\
	LBView.cpp			\
	LBPageView.cpp			\
	LBAppSettings.cpp		\
	LBApplication.cpp		\
	LBIconData.cpp			\
	LBScopeItem.cpp			\
	LBScopeHandler.cpp		\
	LBAlertView.cpp			\
	LBListItem.cpp			\
	LBListView.cpp			\
	LBMenuItem.cpp			\
	LBMenuView.cpp
liblbk_la_LIBADD = ./add-ons/liblbk_add-ons.la
liblbk_la_LDFLAGS = -export-dynamic -rpath $(libdir)

DISTCLEANFILES = Makefile.in

