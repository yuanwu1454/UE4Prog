-- 蓝图名：
-- ##requires
local SuperClass = PureMVC.GamePage
local HUDPage = class('HUDPage', SuperClass)

function HUDPage:ListNeededMediators()
    return {
    }
end

function HUDPage:InitializeLuaEvent()
    SuperClass.InitializeLuaEvent(self)
end

function HUDPage:Construct()
    SuperClass.Construct(self)
end

function HUDPage:Destruct()
    SuperClass.Destruct(self)
end


function HUDPage:OnOpen(luaOpenData, nativeOpenData)
    SuperClass.OnOpen(self, luaOpenData, nativeOpenData)
    self.luaOpenData = luaOpenData
end

function HUDPage:OnClose()
    SuperClass.OnClose(self)
end

return HUDPage