local LoginViewMediator = class("LoginViewMediator", PureMVC.Mediator)
function LoginViewMediator:ListNotificationInterests()
    return {
        ND.LoginShowTestA
    }
end

function LoginViewMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
    if name == ND.LoginShowTestA then
        print("LoginViewMediator LoginShowTestA")
    end
end

function LoginViewMediator:OnRegister()
    LoginViewMediator.super.OnRegister(self)
    LogInfo("LoginViewMediator", "OnRegister")
end

function LoginViewMediator:OnRemove()
    LoginViewMediator.super.OnRemove(self)
    LogInfo("LoginViewMediator", "OnRemove")
end

return LoginViewMediator