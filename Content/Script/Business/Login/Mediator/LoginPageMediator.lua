local LoginPageMediator = class("LoginPageMediator", PureMVC.Mediator)
function LoginPageMediator:ListNotificationInterests()
    return {
        ND.LoginShowTestA
    }
end

function LoginPageMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
    if name == ND.LoginShowTestA then
        print("LoginPageMediator LoginShowTestA")
    end
end

function LoginPageMediator:OnRegister()
    LoginPageMediator.super.OnRegister(self)
    LogInfo("LoginPageMediator", "OnRegister")
end

function LoginPageMediator:OnRemove()
    LoginPageMediator.super.OnRemove(self)
    LogInfo("LoginPageMediator", "OnRemove")
end

return LoginPageMediator