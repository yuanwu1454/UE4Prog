-- 蓝图名：

local SuperClass = PureMVC.GamePanel

local ManaMediator = require("Business/Game/Mediator/ManaMediator")

local StatusProgress = class('StatusProgress', SuperClass)

function StatusProgress:ListNeededMediators()
    LogInfo("StatusProgress", "ListNeededMediators" .. tostring(self.LuaMediatorName))
    return {
        ManaMediator
    }
end

function StatusProgress:InitializeLuaEvent()
    SuperClass.InitializeLuaEvent(self)
end

function StatusProgress:Construct()
    LogInfo("StatusProgress", "Construct")
    SuperClass.Construct(self)
end

function StatusProgress:Destruct()
    LogInfo("StatusProgress", "Destruct")
    SuperClass.Destruct(self)
end

return StatusProgress