local SuperClass = require("Game/Common/LuaActorBase")
local LuaPlayerController = class('LuaPlayerController', SuperClass)

function LuaPlayerController:Initialize(Initializer)
    SuperClass.Initialize(self, Initializer)
    LogInfo(" LuaPlayerController ", "Initialize");
    
end

function LuaPlayerController:UserConstructionScript()
    SuperClass.UserConstructionScript(self)
    LogInfo(" LuaPlayerController ", "UserConstructionScript");
end


function LuaPlayerController:ReceiveBeginPlay()
    SuperClass.ReceiveBeginPlay(self)
    LogInfo(" LuaPlayerController ", "BeginPlay");

    self:EnableInput(self)
    self.bEnableClickEvents = true
    self.bEnableMouseOverEvents = true
end

function LuaPlayerController:ReceiveEndPlay()
    SuperClass.ReceiveEndPlay(self)
    LogInfo(" LuaPlayerController ", "EndPlay");
end

return LuaPlayerController