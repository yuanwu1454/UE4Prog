local LuaCharacterMediator = class("LuaCharacterMediator", PureMVC.Mediator)
function LuaCharacterMediator:ListNotificationInterests()
    return {
        ND.CharacterShowTestA
    }
end

function LuaCharacterMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
    if name == ND.CharacterShowTestA then
        LogInfo(" LuaCharacterMediator ", tostring("Receive Command"));
        
    end
end

function LuaCharacterMediator:OnRegister()
    LuaCharacterMediator.super.OnRegister(self)
    LogInfo("LuaCharacterMediator", "OnRegister")
end

function LuaCharacterMediator:OnRemove()
    LuaCharacterMediator.super.OnRemove(self)
    LogInfo("LuaCharacterMediator", "OnRemove")
end

return LuaCharacterMediator