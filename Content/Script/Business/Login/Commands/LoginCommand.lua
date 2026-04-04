---@class LoginCommand : Command
local LoginCommand = class('LoginCommand', PureMVC.Command)

function LoginCommand:Execute(notification)
    LogInfo("LoginCommand", "Execute")
    local body = notification:GetBody()
    local Type = notification:GetType()
    table.print(body)
    table.print(Type)
    GameFacade:SendNotification(ND.LoginShowTestA)
end


return LoginCommand