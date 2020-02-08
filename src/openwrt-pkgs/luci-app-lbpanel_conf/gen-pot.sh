#!/bin/sh

FILES=" \
./luasrc/controller/lbpanel_conf.lua \
./luasrc/model/cbi/lbpanel_conf.lua \
"

xgettext -ktranslate -o po/templates/lbpanel_conf.pot $FILES

