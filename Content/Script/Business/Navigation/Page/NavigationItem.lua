-- 蓝图名：

local TabSwitchItem = require("Business/Common/Tab/TabSwitchItem")
local NavigationItem = class('NavigationItem',  TabSwitchItem)

function NavigationItem:ListNeededMediators()
    return {
    }
end

function NavigationItem:InitializeLuaEvent()
    TabSwitchItem.InitializeLuaEvent(self)
end

function NavigationItem:LuaHandleKeyEvent(key, inputEvent)
    return false
end

function NavigationItem:Construct()
    TabSwitchItem.Construct(self)
end

function NavigationItem:Destruct()
    TabSwitchItem.Destruct(self)
end


function NavigationItem:RefreshUI()
    self:SetWidgetText(self.Text_Title, self.tabinfo.name)
end

return NavigationItem