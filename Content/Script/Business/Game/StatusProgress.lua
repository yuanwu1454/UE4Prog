-- 蓝图名：
local SuperClass = PureMVC.GamePanel
local StatusProgress = class('StatusProgress', SuperClass)

function StatusProgress:ListNeededMediators()
    LogInfo("StatusProgress", "ListNeededMediators" .. tostring(self.AttrName))
    if self.AttrName == "Mana" then
        local ManaMediator = require("Business/Game/Mediator/ManaMediator")
        return {ManaMediator}
    end

    if self.AttrName == "Health" then
        local HealthMediator = require("Business/Game/Mediator/HealthMediator")
        return {HealthMediator}
    end
end

function StatusProgress:InitializeLuaEvent()
    SuperClass.InitializeLuaEvent(self)
end

function StatusProgress:Construct()
    LogInfo("StatusProgress", "Construct AttrName" .. tostring(self.AttrName))
    SuperClass.Construct(self)
end

function StatusProgress:Destruct()
    LogInfo("StatusProgress", "Destruct")
    SuperClass.Destruct(self)
end

function StatusProgress:UpdateProgress(current, maxMana)
    self:SetWidgetText(self.TextBlock_Progress, string.format("%d/%d", current, maxMana))
    self.ProgressBarUI:SetPercent(current/maxMana)
end

return StatusProgress