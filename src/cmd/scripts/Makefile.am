
SCRIPTS_LBPANEL =		\
	usb-gadget.sh		\
	overlay-switch.sh

EXTRA_DIST = $(SCRIPTS_LBPANEL)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datadir)/scripts
	for f in $(SCRIPTS_LBPANEL) ; do \
		$(INSTALL) -m 700 $$f $(DESTDIR)$(datadir)/scripts; \
	done

uninstall-local:
	for f in $(SCRIPTS_LBPANEL) ; do \
		rm -f $(DESTDIR)$(datadir)/scripts/$$f; \
	done

DISTCLEANFILES = Makefile.in

