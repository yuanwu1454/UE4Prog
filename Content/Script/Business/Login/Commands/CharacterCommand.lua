---@class CharacterCommand : Command
local CharacterCommand = class('CharacterCommand', PureMVC.Command)

function CharacterCommand:Execute(notification)
    LogInfo("CharacterCommand", "Execute")
    local body = notification:GetBody()
    local Type = notification:GetType()
    table.print(body)
    table.print(Type)
    GameFacade:SendNotification(ND.CharacterShowTestA)
end


return CharacterCommand