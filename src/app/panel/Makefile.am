AM_CFLAGS = -I../../include -I../../libs/include
AM_CXXFLAGS = -I../../include -I../../libs/include

noinst_PROGRAMS = lb_panel

lb_panel_SOURCES =		\
	TLogoView.cpp		\
	TLogoView.h		\
	TAboutView.cpp		\
	TAboutView.h		\
	TMainPageView.cpp	\
	TMainPageView.h		\
	TMenuPageView.cpp	\
	TMenuPageView.h		\
	TCommandsPageView.cpp	\
	TCommandsPageView.h	\
	main.cpp
lb_panel_LDADD = ../../libs/lbk/liblbk.la

install-data-local: lb_panel
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(LIBTOOL) --mode=install $(INSTALL) lb_panel $(DESTDIR)$(bindir)/LBPanel

uninstall-local:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(bindir)/LBPanel

EXTRA_DIST = test-panel.sh
DISTCLEANFILES = Makefile.in

