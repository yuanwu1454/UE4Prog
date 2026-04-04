---@class LoginProxy
---@return LoginProxy
local LoginProxy = class("LoginProxy",PureMVC.Proxy)

function LoginProxy:OnRegister()
    LoginProxy.super.OnRegister(self)
end

function LoginProxy:OnRemove()
    LoginProxy.super.OnRemove(self)
end


return LoginProxy