local SuperClass = PureMVC.Mediator
local ManaMediator = class("ManaMediator", SuperClass)
function ManaMediator:ListNotificationInterests()
    return {
        ND.ManaNotify
    }
end

function ManaMediator:ctor(mediatorName,viewComponent)
    SuperClass.ctor(self,mediatorName,viewComponent)
    LogInfo("ManaMediator", "ctor" .. tostring(mediatorName))
end

function ManaMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
    if name == ND.ManaNotify then
        print("ManaMediator ManaNotify")
        table.print(body)
    end
end

function ManaMediator:OnRegister()
    SuperClass.OnRegister(self)
    LogInfo("ManaMediator", "OnRegister", ND.ManaNotify)
end

function ManaMediator:OnRemove()
    SuperClass.OnRemove(self)
    LogInfo("ManaMediator", "OnRemove")
end

return ManaMediator

