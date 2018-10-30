local fs = require "nixio.fs"
local sys = require "luci.sys"

-- -------------------------------------------------------------------------------------------------

-- BEGIN Map
m = Map("usb_otg", translate("USB OTG"), translate("Specify the function of USB OTG."))
function m.on_commit(self)
    sys.exec("/etc/init.d/usb_otg reload")
end
-- END Map

-- BEGIN Settings Section
settings_section = m:section(TypedSection, "usb_otg", translate("Settings"))
settings_section.optional = false
settings_section.rmempty = false
settings_section.anonymous = true
-- END Section

-- BEGIN USB OTG Enable Checkbox
usb_otg_enable = settings_section:option(Flag, "enabled", translate("Enable USB OTG"), "")
usb_otg_enable.optional = false
usb_otg_enable.rmempty = false
-- END USB OTG Enable Checkbox

-- BEGIN Type List
c = settings_section:option(ListValue, "type", translate("Function"))
c.optional = false
c.rmempty = false
c.default = "0"
c:value("0", translate("Ethernet"))
c:value("1", translate("Mass Storage"))
--- END Type List

--- BEGIN g_ether_args
c = settings_section:option(Value, "g_ether_args", translate("Arguments for g_ether"), "")
c.optional = false
c.rmempty = false
--- END g_ether_args

--- BEGIN g_mass_storage_args
c = settings_section:option(Value, "g_mass_storage_args", translate("Arguments for g_mass_storage"), "")
c.optional = false
c.rmempty = false
--- END g_mass_storage_args

return m
