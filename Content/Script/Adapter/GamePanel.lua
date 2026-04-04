---@class GamePanel : UMyBaseUserWidget 
---@return GamePanel
local GamePanel = Class()
---@private
GamePanel.disable_index_set = true

------------------------- 子类关注如下方法-----------------------------
-- 子类在此方法中注册Mediators的类（不需要new出来）
---@protected
function GamePanel:ListNeededMediators()
    return {}
end

-- 在此类中初始各种LuaEvent，供各mediator添加逻辑
---@protected
function GamePanel:InitializeLuaEvent()
end

----------------------------------------------------------------------
-- UserWidget LifeCyle Event
---@private
function GamePanel:OnInitialized()
    LogDebug('GamePanel', 'OnInitialized on %s -- %s', tostring(self), tostring(self.Object))
    -- luaOpenData设置为null
    ---@private
    self.luaData = nil
    -- 子类用来初始化各个LuaEvent的时机
    self:InitializeLuaEvent()
    -- 维一标识（基于地址）
    ---@private
    -- self.UUIDStr = UE4.LuaBridge.LuaGetUniqIdByPointer(self)
    -- if self.UUIDStr == nil then
    --     LogError("GamePanel", "Can not get panel UUID: %s", tostring(self))
    -- end
end

-- 通用注册mediator的方法,mediator名称用类名，GamePanel刚为UserWidget
---@private
function GamePanel:RegisterMediator(mediatorClass)
    -- 用class name注册mediator 为保证mediator的维一性，name需要保持唯一
    -- local ins = mediatorClass.new(mediatorClass.__cname .. self.UUIDStr, self)
    local ins = mediatorClass.new(mediatorClass.__cname, self)
    GameFacade:RegisterMediator(ins)
    -- 引用需要的mediator实例
    table.insert(self.mediators, ins)
end

---@private
function GamePanel:UnRegisterMediator()
    for _, v in ipairs(self.mediators or {}) do
        GameFacade:RemoveMediator(v:GetMediatorName())
    end
    ---@private
    self.mediators = nil
end

-- 注意这个会调用多次,注意和Destruct操作匹配
---@protected
function GamePanel:Construct()
    LogDebug('ViewComponent', self.__cname .. ' -- Construct ' .. tostring(self))
    -- 缓存mediator实例
    ---@private
    self.mediators = {}
    local mediatorClassArr = self:ListNeededMediators()
    for _, v in ipairs(mediatorClassArr) do
        self:RegisterMediator(v)
    end
end

-- 注意这个会调用多次
---@protected
function GamePanel:Destruct()
    LogDebug('ViewComponent', 'Destruct ' .. tostring(self))

    self:RemoveAllBindEvent()
    self:UnRegisterMediator()
    self:RemoveAllTimer()
    ---@private
    self.luaData = nil
end

-- for example
-- 为了避免写很多重复代码
-- if self.Button_Play then
--     self.Button_Play.OnClicked:Add(self, self.OnPlayClick)
-- else
--     LogInfo("GomokuPage", "No Button_Play")
-- end
-- function GamePanel:OnPlayClick()
-- 	self.actionOnPlayClick()
-- end

-- self.actionOnPlayClick = LuaEvent.new()

-- 支持使用代理来处理
function GamePanel:CreateBindEvent(widgetName, widgetDelegateName, bindFuncName, actionEventName, addKey, removeKey)
    addKey = addKey or "Add"
    removeKey = removeKey or "Remove"
    self.bindEventArr = self.bindEventArr or {}
    if self[widgetName] then
        if self[widgetName][widgetDelegateName] then
            local widgetDelegate = self[widgetName][widgetDelegateName]
            local bLuaEvent = false
            if type(widgetDelegate) == "table" and widgetDelegate.__cname == "LuaEvent" then
                bLuaEvent = true
            end

            if widgetDelegate[addKey] then
                local widgetDelegateAddFunc = widgetDelegate[addKey]

                if bindFuncName then
                    if self[bindFuncName] == nil and bLuaEvent == false then
                        LogInfo(self.__cname, string.format("bindFuncName %s not realize: so we auto genereate it", bindFuncName))
                        actionEventName = actionEventName or string.format("action%s", bindFuncName)

                        if actionEventName then
                            if self[actionEventName] == nil then
                                self[actionEventName] = LuaEvent.new()
                            end
        
                            self[bindFuncName] = function (sender, ...)
                                self[actionEventName](...)
                            end

                            if self.mediators then
                                for i, ins in ipairs(self.mediators) do
                                    if ins[bindFuncName] and ins["__bAddActionEvent"..actionEventName] == nil then
                                        self[actionEventName]:Add(ins[bindFuncName], ins)
                                        ins["__bAddActionEvent"..actionEventName] = true
                                    end
                                end
                            end

                        else
                            LogInfo(self.__cname, string.format("actionEventName : %s is nil", actionEventName))
                        end
                    end

                    if self[bindFuncName] then
                        if bLuaEvent then
                            widgetDelegateAddFunc(widgetDelegate, self[bindFuncName], self)
                        else
                            widgetDelegateAddFunc(widgetDelegate, self, self[bindFuncName])
                        end

                        self.bindEventArr[#self.bindEventArr + 1] = {
                            widgetName = widgetName, 
                            widgetDelegateName = widgetDelegateName, 
                            bindFuncName = bindFuncName, 
                            actionEventName = actionEventName, 
                            addKey = addKey, 
                            removeKey = removeKey,
                            bLuaEvent = bLuaEvent
                        }
                    end
                else
                    LogWarn(self.__cname, string.format("bindFuncName : %s is nil", bindFuncName))
                end
            else
                LogWarn(self.__cname, string.format("%s%s%s is nil, can not bind event", widgetName, widgetDelegateName, addKey))
            end
        else
            LogWarn(self.__cname, string.format("%s%s is nil, can not bind event", widgetName, widgetDelegateName))
        end
    else
        LogWarn(self.__cname, string.format("No %s", widgetName))
    end
end

function GamePanel:InnerRemoveBindEvent(v)
    if self[v.widgetName] then
        if self[v.widgetName][v.widgetDelegateName] then
            local widgetDelegate = self[v.widgetName][v.widgetDelegateName]
            if widgetDelegate[v.removeKey] then
                local widgetDelegateRemoveFunc = widgetDelegate[v.removeKey]
                if v.removeKey == "Unbind" then
                    widgetDelegateRemoveFunc(widgetDelegate)
                else
                    if v.bLuaEvent then
                        widgetDelegateRemoveFunc(widgetDelegate, self[v.bindFuncName], self)
                    else
                        widgetDelegateRemoveFunc(widgetDelegate, self, self[v.bindFuncName])
                    end
                end

                -- if v.actionEventName and self[v.actionEventName] then
                --     if self.mediators then
                --         for i, ins in ipairs(self.mediators) do
                --             if ins[v.bindFuncName] then
                --                 self[v.actionEventName]:Remove(ins[v.bindFuncName], ins)
                --             end
                --         end
                --     end
                -- else
                --     LogInfo(self.__cname, string.format("actionEventName : %s is nil", actionEventName))
                -- end
            else
                LogWarn(self.__cname, string.format("%s%s%s is nil, can not bind event", v.widgetName, v.widgetDelegateName, v.removeKey))
            end
        else
            LogWarn(self.__cname, string.format("%s%s is nil, can not bind event", v.widgetName, v.widgetDelegateName))
        end
    else
        LogWarn(self.__cname, string.format("No %s", v.widgetName))
    end
end

-- 可重复调用
function GamePanel:RemoveAllBindEvent()
    if self.bindEventArr then
        for i, v in ipairs(self.bindEventArr) do
            self:InnerRemoveBindEvent(v)
        end
        self.bindEventArr = {}
    end
end

-- 指定widget 来去除
function GamePanel:RemoveBindEventByName(widgetName)
    local removeIndexList = {}
    for i, v in ipairs(self.bindEventArr) do
        if v.widgetName == widgetName then
            self:InnerRemoveBindEvent(v)
            removeIndexList[#removeIndexList + 1] = i
        end
    end

    if #removeIndexList > 0 then
        for i = #removeIndexList, 1, -1 do
            table.remove(self.bindEventArr, removeIndexList[i])
        end
    end
end

-- 定时器业务相关
-- 为了避免由业务自己使用定时器，而在关闭页面时忘记销毁定时器导致的问题 
-- 采用此接口来处理
function GamePanel:AddTimeTask(delayTime, periodTime, maxRunTimes, funcHandle, timeTag)
    self.timeTaskMap = self.timeTaskMap or {}
    self:RemoveTimeTask(timeTag)
    self.timeTaskMap[timeTag] = TimerMgr:AddTimeTask(delayTime, periodTime, maxRunTimes, funcHandle)
end

-- 延时性 定时器
function GamePanel:AddDelayTimeTask(delayTime, funcHandle, timeTag)
    if timeTag == nil then
        self.delayTimeCnt = self.delayTimeCnt or 0
        timeTag = self.delayTimeCnt
        self.delayTimeCnt = self.delayTimeCnt + 1
    end
    self:AddTimeTask(delayTime, 0, 1, funcHandle, timeTag)
    return timeTag
end

-- 周期性 定时器
function GamePanel:AddPeriodTimeTask(periodTime, funcHandle, timeTag, bDelay)
    -- bDelay 表示 延时periodTime 才开始
    if bDelay then
        self:AddTimeTask(periodTime, periodTime, 0, funcHandle, timeTag)
    else
        self:AddTimeTask(0, periodTime, 0, funcHandle, timeTag)
    end
end

-- 周期性 定时器
function GamePanel:RunNextFrame(funcHandle, timeTag)
    self.timeTaskMap = self.timeTaskMap or {}
    self.timeTaskMap[timeTag] = TimerMgr:RunNextFrame(funcHandle)
end

function GamePanel:RemoveTimeTask(timeTag)
    self.timeTaskMap = self.timeTaskMap or {}
    if self.timeTaskMap[timeTag] then
        self.timeTaskMap[timeTag]:EndTask()
        self.timeTaskMap[timeTag] = nil
    end
end

function GamePanel:RemoveAllTimer()
    if self.timeTaskMap then
        for _, v in pairs(self.timeTaskMap) do
            if v then
                v:EndTask()
            end
        end
        self.timeTaskMap = {}
    end
end

-- -- 页面打开动画与页面关闭动画处理
-- 执行动画，但是又想让玩家不与界面内的进行交互
function GamePanel:ShowFullAnim(openAnim, animMask, callback)
    if animMask then
        animMask:SetVisibility(UE4.ESlateVisibility.Visible)
    end

    if openAnim then
        self:PlayAnimation(openAnim, 0, 1, UE4.EUMGSequencePlayMode.Forward, 1, false)
        self:BindToAnimationFinished(
            openAnim,
            function()
                if animMask then
                    animMask:SetVisibility(UE4.ESlateVisibility.Collapsed)
                end
                if callback then
                    callback()
                end
            end
        )
    end
end

function GamePanel:SetWidgetVis(widget, bShow, bInteract)
    if not widget then
        return
    end
    
    if bShow  then
        if bInteract then
            widget:SetVisibility(UE4.ESlateVisibility.Visible)
        else
            self:ShowUWidget(widget)
        end

    else
        self:HideUWidget(widget)
    end
end

function GamePanel:IsWidgetVis(widget, bInteract)
    if widget == nil then
        return false
    end

    if bInteract then
        return widget:GetVisibility() == UE.ESlateVisibility.Visible
    else
        return widget:GetVisibility() ~= UE.ESlateVisibility.Collapsed
    end
end


-- 防止频繁点击
-- function GamePanel:CanExecuteDelayWithTime(delaytime, tag, hintMsg)
--     if tag == nil then
--         LogError(self.__cname, string.format("tag should not be nil"))
--     end
--     self.cdMap = self.cdMap or {}
--     LogInfo(" GamePanel ", tostring(self.cdMap[tag]));
    
--     if self.cdMap[tag] == nil then
--         self.cdMap[tag] = true
--         self:AddDelayTimeTask(delaytime, function ()
--             LogInfo(" GamePanel ", "remove " .. tostring(tag));
--             self.cdMap[tag] = nil
--             self:RemoveTimeTask(tag)
--         end, tag)
--         return true
--     else
--         hintMsg = hintMsg or ConfigMgr:FromStringTable(StringTablePath.ST_Common, "TooFrequent")
--         GameFacade:SendNotification(NotificationDefines.ShowCommonTipCmd, hintMsg)
--         return false
--     end
-- end

-- function GamePanel:PlayLoopAnim(anim, playmode, starttime)
--     playmode = playmode or UE4.EUMGSequencePlayMode.Forward
--     starttime = starttime or 0
--     self:PlayAnimation(anim,starttime,0,UE4.EUMGSequencePlayMode.Forward,1,false)
-- end


-- function GamePanel:BindRedDotEx(RedDotName, callback)
--     self.RedDotMap = self.RedDotMap or {}
--     if self.RedDotMap[RedDotName]~= nil then
--         LogError("GamePanel", tostring(RedDotName) " already had repeat func")
--     end
--     self.RedDotMap[RedDotName] = callback

--     RedDotTree:Bind(RedDotName, callback)

--     if type(callback) == "function" then
--         callback(RedDotTree:GetRedDotCnt(RedDotName))
--     end
-- end

-- function GamePanel:UnBindRedDotEx(RedDotName)
--     self.RedDotMap = self.RedDotMap or {}
--     local callback = self.RedDotMap[RedDotName]
--     if callback then
--         RedDotTree:Unbind(RedDotName, callback)
--     end

-- end

-- function GamePanel:UnBindAllRedDot()
--     self.RedDotMap = self.RedDotMap or {}
--     for RedDotName, callback in pairs(self.RedDotMap) do
--         RedDotTree:Unbind(RedDotName, callback)
--     end
-- end

function GamePanel:SetWidgetActiveIndex(widget, index)
    if widget then
        widget:SetActiveWidgetIndex(index)
    end
end

return GamePanel