local GamePanel = require('Adapter/GamePanel')
---@class GamePage : ViewComponentPanel
---@return GamePage
local GamePage = class('GamePage', GamePanel)

------------------------- 子类关注如下方法-----------------------------
-- 第一次打开
---@protected
function GamePage:OnOpen(luaOpenData, nativeOpenData)

end

---@protected
function GamePage:OnShow(luaOpenData, nativeOpenData)
end

---@protected
function GamePage:OnHide()
end

---@protected
function GamePage:OnClose()
end

---@protected
function GamePage:LuaHandleKeyEvent(key, inputEvent)
    -- 默认行为是关闭自己
    return false
end

----------------------------------------------------------------------
-- UserWidget LifeCyle Event
---@private
function GamePage:NotifyMediatorsPreOpenEvent(...)
    if not self.mediators then
        LogDebug('GamePage', self.__cname .. '--self.mediators = nil')
        return
    end
    for _, v in ipairs(self.mediators) do
        if (v.OnGamePagePreOpen) then
            -- 在打开界面显示前mediator可做一些额外工作
            v:OnGamePagePreOpen(...)
        end
    end
end

---@private
function GamePage:NotifyMediatorsPostOpenEvent(...)
    if not self.mediators then
        LogDebug('GamePage', self.__cname .. '--self.mediators = nil')
        return
    end
    for _, v in ipairs(self.mediators) do
        if (v.OnGamePagePostOpen) then
            -- 在打开界面mediator可做一些额外工作
            v:OnGamePagePostOpen(...)
        end
    end
end

-- 获取Lua传递过来的打数据及原始的数据Opendata
---@protected
function GamePage:GetOpenData()
    local openData = self:NativeGetOpenData()
    LogDebug('GamePage', 'NativeGetOpenData Result %s', tostring(openData))
    if (not self.luaData) then
        if (openData and openData.LuaOpenDataRef) then
            self.luaData = UE4.LuaBridge.LuaGetRefObject(openData.LuaOpenDataRef)
        end
    end
    return self.luaData, openData
end


-- 子类不要重写此方法，用OnOpen,Show,Hide,Close方法同样如此
---@private
function GamePage:OnLuaOpen()
    LogDebug('GamePage', 'OnLuaOpen')



    local luaData, originOpenData = self:GetOpenData()
    local openStr = tostring(self.__cname) .. "OnLuaOpen"
    table.print(luaData, openStr)
    print("originOpenData", originOpenData)

    self:NotifyMediatorsPreOpenEvent(luaData, originOpenData)
    self:OnOpen(luaData, originOpenData)
    self:NotifyMediatorsPostOpenEvent(luaData, originOpenData)
end

-- 展示
---@private
function GamePage:OnLuaShow()
    LogDebug('GamePage', 'OnLuaShow')
    local luaData, originOpenData = self:GetOpenData()
    self:OnShow(luaData, originOpenData)
end

-- 隐藏
---@private
function GamePage:OnLuaHide()
    LogDebug('GamePage', 'OnLuaHide')
    self:OnHide()
end

-- 永久关闭
---@private
function GamePage:OnLuaClose()
    LogDebug('GamePage', 'OnLuaClose')
    self:OnClose()
end

return GamePage