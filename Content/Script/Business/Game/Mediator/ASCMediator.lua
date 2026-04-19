local SuperClass = PureMVC.Mediator
local ASCMediator = class("ASCMediator", SuperClass)
function ASCMediator:ListNotificationInterests()
    return {
        ND.InitASCNotify,
    }
end

function ASCMediator:ctor(mediatorName,viewComponent)
    SuperClass.ctor(self,mediatorName,viewComponent)
end

function ASCMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
    local view = self:GetViewComponent();
    if name == ND.InitASCNotify then
        local asc = body[1]
        if self:IsOwnerASC(asc) then
            self:Init()
        end
    end
end

function ASCMediator:OnRegister()
    SuperClass.OnRegister(self)
    if self:IsASCInitialized() then
        self:Init()
    end
end

function ASCMediator:OnRemove()
    SuperClass.OnRemove(self)
    LogInfo("ASCMediator", "OnRemove")
end

function ASCMediator:IsASCInitialized()
    local view = self:GetViewComponent()
    return UE4.UGASFunctionLibrary.IsASCInitialized(view:GetOwningPlayerPawn())
end

function ASCMediator:IsOwnerASC(asc)
    local view = self:GetViewComponent()
    return UE4.UGASFunctionLibrary.IsOwnerBy(view:GetOwningPlayerPawn(), asc)
end

function ASCMediator:Init()
end

return ASCMediator

