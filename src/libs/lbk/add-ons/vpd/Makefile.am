AM_CFLAGS = -fPIC -I../../../../include -I../../../include
AM_CXXFLAGS = -fPIC -I../../../../include -I../../../include
AM_LDFLAGS = -shared

addonsdir = $(libdir)/add-ons/lbk

noinst_LTLIBRARIES = libvpd.la

libvpd_la_SOURCES =		\
	font8.h			\
	VPD.cpp			\
	VPD.h			\
	VPDApp.cpp		\
	VPDApp.h		\
	VPDView.cpp		\
	VPDView.h
libvpd_la_LDFLAGS = -export-dynamic -rpath $(libdir)

install-data-local: libvpd.la
	$(mkinstalldirs) $(DESTDIR)$(addonsdir)
	$(LIBTOOL) --mode=install $(INSTALL) libvpd.la $(DESTDIR)$(addonsdir)/vpd.la

uninstall-local:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(addonsdir)/vpd.la

DISTCLEANFILES = Makefile.in

