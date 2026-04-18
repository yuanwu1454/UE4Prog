local SuperClass = require("Game/Common/LuaActorBase")
local HUDBase = class('HUDBase', SuperClass)
function HUDBase:ReceiveBeginPlay()
    SuperClass.ReceiveBeginPlay(self)
    LogInfo(" HUDBase ", "BeginPlay");
end

function HUDBase:ReceiveEndPlay()
    LogInfo(" HUDBase ", "EndPlay");
    SuperClass.ReceiveEndPlay(self)
end

return HUDBase