
local SuperClass = require("Game.Common.LuaObjectBase")
local LuaGameInstance = class('LuaGameInstance', SuperClass)

function LuaGameInstance:ReceiveInit()
    LogInfo(" LuaGameInstance ", tostring("ReceiveInit"));
end

function LuaGameInstance:ReceiveShutdown()
    LogInfo(" LuaGameInstance ", tostring("ReceiveShutdown"));
end

function LuaGameInstance:HandleTravelError(failuretype)
    LogInfo(" LuaGameInstance HandleTravelError", tostring(failuretype));
end


function LuaGameInstance:HandleNetworkError(failuretype, isServer)
    LogInfo(" LuaGameInstance HandleNetworkError", tostring(failuretype) .. tostring(isServer));
end

return LuaGameInstance
