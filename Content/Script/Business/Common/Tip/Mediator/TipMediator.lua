local TipMediator = class("TipMediator", PureMVC.Mediator)

function TipMediator:ListNotificationInterests()
    return {
        _G.ND.ShowCommonTip
    }
end

function TipMediator:HandleNotification(notification)
    local NtfName = notification:GetName()
    local body = notification:GetBody()
    if NtfName == _G.ND.ShowCommonTip then
        self:GetViewComponent():ShowMsg(body.realMsg, body.extra)
    end
end

return TipMediator
