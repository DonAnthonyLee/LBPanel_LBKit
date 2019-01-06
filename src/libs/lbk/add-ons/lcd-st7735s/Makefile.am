AM_CFLAGS = -I../../../../include -I../../../include
AM_CXXFLAGS = -I../../../../include -I../../../include
AM_LDFLAGS = -shared

addonsdir = $(libdir)/add-ons/lbk

noinst_LTLIBRARIES = liblcd-st7735s.la

liblcd_st7735s_la_SOURCES =	\
	LCDScreen.cpp		\
	LCDScreen.h
liblcd_st7735s_la_LDFLAGS = -export-dynamic -rpath $(libdir)

install-data-local: liblcd-st7735s.la
	$(mkinstalldirs) $(DESTDIR)$(addonsdir)
	$(LIBTOOL) --mode=install $(INSTALL) liblcd-st7735s.la $(DESTDIR)$(addonsdir)/lcd-st7735s.la

uninstall-local:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(addonsdir)/lcd-st7735s.la

DISTCLEANFILES = Makefile.in

