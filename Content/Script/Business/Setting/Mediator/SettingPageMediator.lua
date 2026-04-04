local SettingPageMediator = class("SettingPageMediator", PureMVC.Mediator)
function SettingPageMediator:ListNotificationInterests()
    return {
    }
end

function SettingPageMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
end

function SettingPageMediator:OnRegister()
    SettingPageMediator.super.OnRegister(self)
    LogInfo("SettingPageMediator", "OnRegister")
end

function SettingPageMediator:OnRemove()
    SettingPageMediator.super.OnRemove(self)
    LogInfo("SettingPageMediator", "OnRemove")
end

return SettingPageMediator