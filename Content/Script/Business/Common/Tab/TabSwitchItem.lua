-- 蓝图名：

local SuperClass = PureMVC.GamePanel

local TabSwitchItem = class('TabSwitchItem', SuperClass)

function TabSwitchItem:InitializeLuaEvent()
	self.actionOnItemClick = LuaEvent.new()
    self.isSelected = false
    SuperClass.InitializeLuaEvent(self)
end

function TabSwitchItem:Construct()
    self:CreateBindEvent("ButtonClick", "OnClicked", "OnItemClicked")
    SuperClass.Construct(self)
end

function TabSwitchItem:Destruct()
    SuperClass.Destruct(self)
end

function TabSwitchItem:UpdateData(info, index)
    self.tabinfo = info
    self.index = index
    self:InitRedDot()

    self:RefreshUI()
end

function TabSwitchItem:InitRedDot()
	local redDotKey = self:GetRedDotKey()
    if redDotKey then
        self:BindRedDotEx(RedDotModuleDef.ModuleName[redDotKey], function (cnt)
            self:UpdateRedDot(cnt)
        end)
    else
        self:SetWidgetVis(self.RedDot, false)
    end
end


function TabSwitchItem:OnItemClicked()
    self.actionOnItemClick(self.index)
end

function TabSwitchItem:UpdateRedDot(cnt)
    self:SetWidgetVis(self.RedDot, cnt > 1)
end

function TabSwitchItem:SetSelectState(isSelect)
	if self.isSelected == isSelect then 
		return
	end
	self.isSelected = isSelect
    self:UpdateSelectUI()
end

-- override 刷新ui
function TabSwitchItem:RefreshUI()

end

-- override 选择效果
function TabSwitchItem:UpdateSelectUI()

end

-- override 红点key
function TabSwitchItem:GetRedDotKey()
    return 
end


return TabSwitchItem