module("luci.controller.usb_otg", package.seeall)

function index()
    if not nixio.fs.access("/etc/config/usb_otg") then
        return
    end
    local page
    page = entry({"admin", "system", "usb_otg"}, cbi("usb_otg"), _("USB OTG"))
    page.i18n = "usb_otg"
    page.dependent = true
end

