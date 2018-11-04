AM_CFLAGS = -fPIC -I../../../../include -I../../../include
AM_CXXFLAGS = -fPIC -I../../../../include -I../../../include
AM_LDFLAGS = -shared

addonsdir = $(libdir)/add-ons/lbk

noinst_LTLIBRARIES = libnpi-oled-hat.la

libnpi_oled_hat_la_SOURCES =	\
	Config.h		\
	NPIHat.cpp		\
	NPIHat.h
libnpi_oled_hat_la_LDFLAGS = -export-dynamic -rpath $(libdir)

install-data-local: libnpi-oled-hat.la
	$(mkinstalldirs) $(DESTDIR)$(addonsdir)
	$(LIBTOOL) --mode=install $(INSTALL) libnpi-oled-hat.la $(DESTDIR)$(addonsdir)/npi-oled-hat.la

uninstall-local:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(addonsdir)/npi-oled-hat.la

DISTCLEANFILES = Makefile.in

