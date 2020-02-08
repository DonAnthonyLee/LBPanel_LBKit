#!/bin/sh

FILES=" \
./luasrc/controller/usb_gadget.lua \
./luasrc/model/cbi/usb_gadget.lua \
"

xgettext -ktranslate -o po/templates/usb_gadget.pot $FILES

