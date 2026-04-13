_G.ShowTip = function (msg, extra)
    extra = extra or {}
    local context = extra.context or LuaGetWorld()


    if _G.bShowTipPage then
        GameFacade:SendNotification(ND.ShowCommonTip,  {realMsg = msg, extra = extra})
    else
        ViewMgr:OpenPage(context, "TipPage",nil, {realMsg = msg, extra = extra})
    end
end