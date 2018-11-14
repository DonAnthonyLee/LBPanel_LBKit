local fs = require "nixio.fs"
local sys = require "luci.sys"

-- -------------------------------------------------------------------------------------------------

local m, s, c

-- BEGIN Map
m = Map("lbpanel_conf", translate("LBPanel"), translate("Little board panel application"))
function m.on_commit(self)
    sys.exec("/etc/init.d/lbpanel reload_uci_config")
end
-- END Map

-- BEGIN Section
s = m:section(TypedSection, "main", translate("Screen settings"))
s.optional = false
s.rmempty = false
s.anonymous = true
-- END Section

--- BEGIN screen_timeout
c = d:option(Value, "screen_off_timeout", translate("Screen off timeout (s)"), translate("0 to disable, minimum value: 10."))
c.optional = false
c.datatype = "range(0,600)"
c.rmempty = false
--- END screen_timeout

-- BEGIN Checkbox
c = s:option(Flag, "hours", translate("24 hours"), "")
c.optional = false
c.rmempty = false
-- END Checkbox

-- BEGIN Checkbox
c = s:option(Flag, "show_seconds", translate("Show seconds"), "")
c.optional = false
c.rmempty = false
-- END Checkbox

-- BEGIN Checkbox
c = s:option(Flag, "lcd_style", translate("LCD style"), "")
c.optional = false
c.rmempty = false
-- END Checkbox

-- BEGIN Section
s = m:section(TypedSection, "menuitem", "")
s.template = "cbi/tblsection"
s.anonymous = true
s.addremove = true
-- END Section

s:option(Value, "title", translate("Item title"),
         translate("Title to show on custom menu"))

s:option(Value, "command", translate("Command"),
         translate("Command line to execute"))

s:option(Value, "args", translate("Arguments"),
         translate("Additional command line arguments"))

return m
