-- 蓝图名：
-- ##requires
local SuperClass = PureMVC.GamePage
local TabSwitchPage = class('TabSwitchPage', SuperClass)

function TabSwitchPage:ListNeededMediators()
    return {
    }
end

function TabSwitchPage:InitializeLuaEvent()
    SuperClass.InitializeLuaEvent(self)
end

function TabSwitchPage:LuaHandleKeyEvent(key, inputEvent)
    return self.HotKeyButton_Esc:MonitorKeyDown(key, inputEvent)
end

function TabSwitchPage:Construct()
    SuperClass.Construct(self)

    self.bHideOnlyOne = false

    self:CreateBindEvent("HotKeyButton_Esc", "OnClickEvent", "OnEscHotKeyClick")
end

function TabSwitchPage:Destruct()
    SuperClass.Destruct(self)
end


function TabSwitchPage:OnOpen(luaOpenData, nativeOpenData)
    self.luaOpenData = luaOpenData
    self:SetLastOpenData(luaOpenData)
    local lastOpenData = self:GetLastOpenData()


    -- 播放动画
	if self.OpenAnimation then
		self:PlayAnimation(self.OpenAnimation)
	end

	self.cachedPageDict = {}
	if self.SwitchAnimation then
		self.SwitchAnimation:PlayOpenAnimation({self,function()
		end})
	end

    self:InitTabs(lastOpenData)
end

function TabSwitchPage:OnClose()
     self:SwitchPage(self.curOpenPage, false)
     self.curOpenPage = nil

     if self.cachedPageDict then
		for _, widget in pairs(self.cachedPageDict) do
			widget:RemoveFromParent()
		end
		self.cachedPageDict = nil
	end
end

function TabSwitchPage:PrivateSelectTab(index)
    local items = self:GetTabs():GetAllEntries()
    for i = 1 , items:Length() do
        local item = items:Get(i)
        item:SetSelectState(index == i)
    end

    if self.curLotteryId == index then
		return
	end
	self.curLotteryId = index

    local id = self:GetID(index)
    self:SetLastOpenData(id)
	self:OnOpenSubPage(id)
end

function TabSwitchPage:SelectTab(index)
    self:PrivateSelectTab(index)
end


function TabSwitchPage:OnOpenSubPage(id)
    self:SwitchPage(self.curOpenPage, false)
    self.curOpenPage = self:GetPage(id)
    self:SwitchPage(self.curOpenPage, true)
end

function TabSwitchPage:SwitchPage(page, bOpen)
	if page then
        if bOpen then
            self:SetWidgetVis(page, bOpen)
            self:PrivateOpenPage(page)
            page.bCurShow = true
        else
            page.bCurShow = false
            if page.OnClose then
                page:OnClose()
            end
            self:SetWidgetVis(page, bOpen)
        end
	end
end

function TabSwitchPage:GetPage(id)
    self.currentId = id
    if self.cachedPageDict[id] == nil then
        local pageClass = self:GetPageClass(id)
        if not pageClass or pageClass:IsNull() or not pageClass:LoadSynchronous() then
            self:SetWidgetVis(self.EmptyPage, true)
        else
            self:SetWidgetVis(self.EmptyPage, false)
            if self.cachedPageDict[id] == nil then
                self:InitPage(id, pageClass)
            end
        end
    end

    return self.cachedPageDict[id]
end

function TabSwitchPage:InitPage(id, pageClass)
	local page = self.cachedPageDict[id]

	if not page then
        page = UE4.UWidgetBlueprintLibrary.Create(self, pageClass:LoadSynchronous())
		self.cachedPageDict[id] = page
		local slot = self.Overlay_Child:AddChildToOverlay(page)
		slot:SetHorizontalAlignment(UE4.EHorizontalAlignment.HAlign_Fill)
		slot:SetVerticalAlignment(UE4.EVerticalAlignment.VAlign_Fill)
	end
end


function TabSwitchPage:InitTabs(defaultSelectID)
    self:PrviateInitTab()

    local idToSelect = self:GetIdToSelect(defaultSelectID)
	self:SelectTab(idToSelect)
end

function TabSwitchPage:PrviateInitTab()
    self:GetTabs():Reset(true)
    self.tabInfos = self:GetTabInfos()

	for i, tabInfo in ipairs(self.tabInfos) do
		local tabWidget = self:GetTabs():BP_CreateEntry()
		tabWidget:UpdateData(tabInfo, i)
		tabWidget.actionOnItemClick:Add(self.SelectTab, self)
	end

    if self.bHideOnlyOne and #self.tabInfos <= 1 then
        self:SetWidgetVis(self:GetTabs(), false)
        self:SetWidgetVis(self.ScrollBox_Entry, false)
    end
end

function TabSwitchPage:GetIdToSelect(defaultId)
    local idToSelect = self:GetIndex(defaultId)
    return idToSelect
end

function TabSwitchPage:GetIndex(id)
    for i, v in ipairs(self.tabInfos) do
        if self:Cmp(v, id) then
            return i
        end
    end
    return 1
end

-- override 根据传入的index 获取 应该得到的id， 这个id 是唯一标识。
function TabSwitchPage:GetID(index)
    if self.tabInfos[index] then

    end
end

-- override 根据传入的defaultid 与 自己tabInfo 判定是否相等
function TabSwitchPage:Cmp(tabInfo, defaultId)
    return false
end

-- override 获取各个tab的信息
function TabSwitchPage:GetTabInfos()
    return {}
end

-- override 根据id 获取page class
function TabSwitchPage:GetPageClass(id)

end

-- 用于设置上次打开页面的记录
function TabSwitchPage:GetLastOpenData()
    local cls = _G.getClass(self.__cname)
    return cls.LastOpenData
end

function TabSwitchPage:SetLastOpenData(var)
    if var then
        local cls = _G.getClass(self.__cname)
        cls.LastOpenData = var
    end
end

function TabSwitchPage:OnEscHotKeyClick()
    GameFacade:SendNotification(NotificationDefines.NavigationBar.SwitchDisplayPage)
end

function TabSwitchPage:GetTabs()
    return self.DynamicEntryBox_Tabs
end

function TabSwitchPage:PrivateOpenPage(page)
    if page.OnOpen then
        page:OnOpen(self.currentId)
    end
end

return TabSwitchPage