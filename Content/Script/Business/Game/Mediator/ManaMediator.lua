local SuperClass = require("Business/Game/Mediator/ASCMediator")
local ManaMediator = class("ManaMediator", SuperClass)
function ManaMediator:ListNotificationInterests()
    local ret = SuperClass.ListNotificationInterests(self)
    table.insert(ret, ND.ManaNotify)
    return ret
end

function ManaMediator:ctor(mediatorName,viewComponent)
    SuperClass.ctor(self,mediatorName,viewComponent)
end

function ManaMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
    local view = self:GetViewComponent();
    SuperClass.HandleNotification(self, notification)
    if name == ND.ManaNotify then
        if type(body) == "table" then
            local asc = body[1]
            local preValue = body[2]
            local postValue = body[3]
            if preValue and postValue and preValue ~= postValue and self:IsOwnerASC(asc) then
                self.currentMana = postValue
                view:UpdateProgress(self.currentMana, self.maxMana)
            end
        end
    end
end

function ManaMediator:OnRegister()
    SuperClass.OnRegister(self)
end

function ManaMediator:InitMana()
    local view = self:GetViewComponent()
    local bCurrentOK, currentMana = UE4.UGASFunctionLibrary.GetCurrentMana(view:GetOwningPlayerPawn())
    local bMaxOK, maxMana = UE4.UGASFunctionLibrary.GetMaxMana(view:GetOwningPlayerPawn())
    if bCurrentOK and bMaxOK then
        self.currentMana = currentMana
        self.maxMana = maxMana
        view:UpdateProgress(currentMana, maxMana)
    end
end

function ManaMediator:Init()
    SuperClass.Init(self)
    self:InitMana()
end

function ManaMediator:OnRemove()
    SuperClass.OnRemove(self)
    LogInfo("ManaMediator", "OnRemove")
end

return ManaMediator

