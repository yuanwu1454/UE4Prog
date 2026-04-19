local LuaObjectBase = Class()
LuaObjectBase.disable_index_set = true

function LuaObjectBase:ListNeededMediators()
    return {}
end

function LuaObjectBase:InitMediator()
    self.mediators = {}
    local mediatorClassArr = self:ListNeededMediators()
    for _, v in ipairs(mediatorClassArr) do
        self:RegisterMediator(v)
    end
end


-- 通用注册mediator的方法,mediator名称用类名，LuaObjectBase刚为UserWidget
---@private
function LuaObjectBase:RegisterMediator(mediatorClass)
    -- 用class name注册mediator 为保证mediator的维一性，name需要保持唯一
    -- local ins = mediatorClass.new(mediatorClass.__cname .. self.UUIDStr, self)
    local ins = mediatorClass.new(mediatorClass.__cname .. tostring(self), self)
    GameFacade:RegisterMediator(ins)
    -- 引用需要的mediator实例
    table.insert(self.mediators, ins)
end

---@private
function LuaObjectBase:UnRegisterMediator()
    for _, v in ipairs(self.mediators or {}) do
        GameFacade:RemoveMediator(v:GetMediatorName())
    end
    ---@private
    self.mediators = nil
end


-- 定时器业务相关
-- 为了避免由业务自己使用定时器，而在关闭页面时忘记销毁定时器导致的问题 
-- 采用此接口来处理
function LuaObjectBase:AddTimeTask(delayTime, periodTime, maxRunTimes, funcHandle, timeTag)
    self.timeTaskMap = self.timeTaskMap or {}
    self:RemoveTimeTask(timeTag)
    self.timeTaskMap[timeTag] = TimerMgr:AddTimeTask(delayTime, periodTime, maxRunTimes, funcHandle)
end

-- 延时性 定时器
function LuaObjectBase:AddDelayTimeTask(delayTime, funcHandle, timeTag)
    if timeTag == nil then
        self.delayTimeCnt = self.delayTimeCnt or 0
        timeTag = self.delayTimeCnt
        self.delayTimeCnt = self.delayTimeCnt + 1
    end
    self:AddTimeTask(delayTime, 0, 1, funcHandle, timeTag)
    return timeTag
end

-- 周期性 定时器
function LuaObjectBase:AddPeriodTimeTask(periodTime, funcHandle, timeTag, bDelay)
    -- bDelay 表示 延时periodTime 才开始
    if bDelay then
        self:AddTimeTask(periodTime, periodTime, 0, funcHandle, timeTag)
    else
        self:AddTimeTask(0, periodTime, 0, funcHandle, timeTag)
    end
end

-- 周期性 定时器
function LuaObjectBase:RunNextFrame(funcHandle, timeTag)
    self.timeTaskMap = self.timeTaskMap or {}
    self.timeTaskMap[timeTag] = TimerMgr:RunNextFrame(funcHandle)
end

function LuaObjectBase:RemoveTimeTask(timeTag)
    self.timeTaskMap = self.timeTaskMap or {}
    if self.timeTaskMap[timeTag] then
        self.timeTaskMap[timeTag]:EndTask()
        self.timeTaskMap[timeTag] = nil
    end
end

function LuaObjectBase:RemoveAllTimer()
    if self.timeTaskMap then
        for _, v in pairs(self.timeTaskMap) do
            if v then
                v:EndTask()
            end
        end
        self.timeTaskMap = {}
    end
end

function LuaObjectBase:DoMediatorByFunc(funcName)
    for i, v in ipairs(self.mediators or {}) do
        if v[funcName] then
            v[funcName](v)
        end
    end
end


return LuaObjectBase