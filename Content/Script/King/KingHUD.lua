local SuperClass = require("UE/HUDBase")
local KingHUD = class('KingHUD', SuperClass)

function KingHUD:ReceiveBeginPlay()
    SuperClass.ReceiveBeginPlay(self)
    LogInfo(" KingHUD ", "BeginPlay");

    ViewMgr:OpenPage(LuaGetWorld(), "HUDPage")
end

function KingHUD:ReceiveEndPlay()
    LogInfo(" KingHUD ", "EndPlay");
    SuperClass.ReceiveEndPlay(self)
end

return KingHUD
