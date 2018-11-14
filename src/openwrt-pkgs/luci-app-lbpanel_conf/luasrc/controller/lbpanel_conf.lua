module("luci.controller.lbpanel_conf", package.seeall)

function index()
    if not nixio.fs.access("/etc/config/lbpanel_conf") then
        return
    end
    local page
    page = entry({"admin", "system", "lbpanel_conf"}, cbi("lbpanel_conf"), _("LBPanel"))
    page.i18n = "lbpanel_conf"
    page.dependent = true
end

