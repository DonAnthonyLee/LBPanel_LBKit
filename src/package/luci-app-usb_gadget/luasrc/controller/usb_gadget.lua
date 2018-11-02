module("luci.controller.usb_gadget", package.seeall)

function index()
    if not nixio.fs.access("/etc/config/usb_gadget") then
        return
    end
    local page
    page = entry({"admin", "system", "usb_gadget"}, cbi("usb_gadget"), _("USB Gadget"))
    page.i18n = "usb_gadget"
    page.dependent = true
end

