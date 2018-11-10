
SCRIPTS_LBPANEL =		\
	usb-gadget.sh		\
	overlay-switch.sh

EXTRA_DIST = $(SCRIPTS_LBPANEL)

install-data-local: libnpi-oled-hat.la
	$(mkinstalldirs) $(DESTDIR)$(datadir)/lbpanel/scripts
	for f in $(SCRIPTS_LBPANEL); do \
		$(INSTALL) -m 700 ./$$f $(DESTDIR)$(datadir)/lbpanel/scripts \
	done

uninstall-local:
	rm -rf $(DESTDIR)$(datadir)/lbpanel/scripts

DISTCLEANFILES = Makefile.in

