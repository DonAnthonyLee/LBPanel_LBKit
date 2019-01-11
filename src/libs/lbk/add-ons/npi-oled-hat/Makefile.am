AM_CFLAGS = -I../../../../include -I../../../include
AM_CXXFLAGS = -I../../../../include -I../../../include
AM_LDFLAGS = -shared

NPI_HAT_USE_COMBINER = y

addonsdir = $(libdir)/add-ons/lbk

noinst_LTLIBRARIES = libnpi-oled-hat.la

ifeq ($(NPI_HAT_USE_COMBINER), y)
libnpi_oled_hat_la_SOURCES =	\
	Config.h		\
	NPIHat.cpp		\
	NPIHat.h
else
libnpi_oled_hat_la_SOURCES =	\
	Config.h		\
	NPIHatCombiner.cpp	\
	NPIHatCombiner.h
endif

libnpi_oled_hat_la_LDFLAGS = -export-dynamic -rpath $(libdir)

install-data-local: libnpi-oled-hat.la
	$(mkinstalldirs) $(DESTDIR)$(addonsdir)
	$(LIBTOOL) --mode=install $(INSTALL) libnpi-oled-hat.la $(DESTDIR)$(addonsdir)/npi-oled-hat.la

uninstall-local:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(addonsdir)/npi-oled-hat.la

DISTCLEANFILES = Makefile.in

