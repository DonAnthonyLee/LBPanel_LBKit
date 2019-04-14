AM_CFLAGS = -I../../../../include -I../../../include
AM_CXXFLAGS = -I../../../../include -I../../../include
AM_LDFLAGS = -shared

addonsdir = $(libdir)/add-ons/lbk

noinst_LTLIBRARIES = libkeypad-generic.la

libkeypad_generic_la_SOURCES =	\
	KeypadGeneric.cpp	\
	KeypadGeneric.h
libkeypad_generic_la_LDFLAGS = -export-dynamic -rpath $(libdir)

install-data-local: libkeypad-generic.la
	$(mkinstalldirs) $(DESTDIR)$(addonsdir)
	$(LIBTOOL) --mode=install $(INSTALL) libkeypad-generic.la $(DESTDIR)$(addonsdir)/keypad-generic.la

uninstall-local:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(addonsdir)/keypad-generic.la

DISTCLEANFILES = Makefile.in

