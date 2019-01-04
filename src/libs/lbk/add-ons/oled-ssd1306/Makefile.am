AM_CFLAGS = -I../../../../include -I../../../include
AM_CXXFLAGS = -I../../../../include -I../../../include
AM_LDFLAGS = -shared

addonsdir = $(libdir)/add-ons/lbk

noinst_LTLIBRARIES = liboled-ssd1306.la

liboled_ssd1306_la_SOURCES =	\
	OLEDScreen.cpp		\
	OLEDScreen.h
liboled_ssd1306_la_LDFLAGS = -export-dynamic -rpath $(libdir)

install-data-local: liboled-ssd1306.la
	$(mkinstalldirs) $(DESTDIR)$(addonsdir)
	$(LIBTOOL) --mode=install $(INSTALL) liboled-ssd1306.la $(DESTDIR)$(addonsdir)/oled-ssd1306.la

uninstall-local:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(addonsdir)/oled-ssd1306.la

DISTCLEANFILES = Makefile.in

