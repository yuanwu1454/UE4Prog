---@class ViewMgr
local ViewMgr = {}


-- worldObject传PageWidget
---@param worldObject string |UIPageNameDefineViewMgr|
function ViewMgr:OpenPage(worldObject, name, newIns, openData)
    UE4.LuaBridge.LuaOpenPage(worldObject, name, newIns, openData)
end

-- 参数1 （必须） WorldObject一般为Widget
-- 参数2  (选择)  配置表格中的PageName  当第二个参数为空时，会默认关闭第一个参数传入的Widget
function ViewMgr:ClosePage(worldObject, uiPageName)
    UE4.LuaBridge.LuaClosePage(worldObject, uiPageName)
end

---@param worldObject UObject
---@param uiName string
---@return UPMBaseUserWidget
function ViewMgr:FindPageView(worldObject, uiName)
	return UE4.LuaBridge.LuaFindPageView(worldObject, uiName)
end

-- 参数1 （必须） WorldObject一般为Widget
-- 参数2  (选择)  配置表格中的PageName
function ViewMgr:HidePage(worldObject, uiPageName)
    UE4.LuaBridge.LuaHidePage(worldObject, uiPageName)
end

_G.ViewMgr = ViewMgr
