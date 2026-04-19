local SuperClass = require("Business/Game/Mediator/ASCMediator")
local HealthMediator = class("HealthMediator", SuperClass)
function HealthMediator:ListNotificationInterests()
    local ret = SuperClass.ListNotificationInterests(self)
    table.insert(ret, ND.HealthNotify)
    return ret
end

function HealthMediator:ctor(mediatorName,viewComponent)
    SuperClass.ctor(self,mediatorName,viewComponent)
    LogInfo("HealthMediator", "ctor" .. tostring(mediatorName))
end

function HealthMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
    local view = self:GetViewComponent();
    SuperClass.HandleNotification(self, notification)
    if name == ND.HealthNotify then
        print("HealthMediator HealthNotify")
        table.print(body)
        if type(body) == "table" then
            local asc = body[1]
            local preValue = body[2]
            local postValue = body[3]
            if preValue and postValue and preValue ~= postValue and self:IsOwnerASC(asc) then
                self.currentHealth = postValue
                view:UpdateProgress(self.currentHealth, self.maxHealth)
            end
        end
    end
end

function HealthMediator:Init()
    SuperClass.Init(self)
    self:InitHealth()
end

function HealthMediator:InitHealth()
    print("InitHealth")
    local view = self:GetViewComponent()
    local bCurrentOK, currentHealth = UE4.UGASFunctionLibrary.GetCurrentHealth(view:GetOwningPlayerPawn())
    local bMaxOK, maxHealth = UE4.UGASFunctionLibrary.GetMaxHealth(view:GetOwningPlayerPawn())
    if bCurrentOK and bMaxOK then
        self.currentHealth = currentHealth
        self.maxHealth = maxHealth
        view:UpdateProgress(currentHealth, maxHealth)
    end
end

function HealthMediator:OnRegister()
    SuperClass.OnRegister(self)
end

function HealthMediator:OnRemove()
    SuperClass.OnRemove(self)
end

return HealthMediator

