require("Global.LuaEvent")
require("Global.FuncSlot")
-- 同次数有的任务
-- 单位如果时时间则与时间相关，如果按帧，则每次Tickdelta 1
---@class TimesRelatedTask
local TimesRelatedTask = class('TimesRelatedTask')
-- delay: 首次执行的时间
-- period: 首次执行后的周期执行时间
-- 最多执行的次数
function TimesRelatedTask:ctor(delay, period, runTimes, funcHandle, isFrame)
    self.delay = delay or 0 -- 默认立即执行
    self.period = period or 0   -- 周期
    self.runTimes = runTimes or 1 -- 最多执行的次数默认1次
    self.funcHandle = funcHandle

    self.accumulateTime = nil
    self.haveRunTimes = 0
    self.firstRun = false     -- 第一次执行的标识
    self.bFinished = false
    self.bPaused = false

    -- 立即第一次执行
    if (delay == 0) then
        if isFrame then 
            self.funcHandle(0)
        else
            self.funcHandle()
        end
        self.haveRunTimes = 1
        self.firstRun = true
        if (self.runTimes > 0 and self.runTimes <= 1) then  -- 只执行一次，可以立即结束
            self.bFinished = true
        end
    end
    self.accumulateTime = 0
end
function TimesRelatedTask:FinishedTask()
    self.funcHandle()
    self.bFinished = true
end

function TimesRelatedTask:EndTask()
    self.bFinished = true
end

function TimesRelatedTask:IsFinished()
    return self.bFinished
end

function TimesRelatedTask:PauseTask()
    self.bPaused = true
end

function TimesRelatedTask:UnPauseTask()
    self.bPaused = false
end

function TimesRelatedTask:IsPaused()
    return self.bPaused
end

function TimesRelatedTask:Tick(deltaTime)
    if (self.bFinished and self.bPaused) then
        return
    end
    self.accumulateTime = self.accumulateTime + deltaTime
    if (not self.firstRun) then
        if (self.accumulateTime >= self.delay) then
            self.funcHandle()
            self.haveRunTimes = 1
            self.firstRun = true
			self.accumulateTime = 0
        end
    else
        -- 周期执行
        if (self.period <= 0) then
            self.bFinished = true
        else
            if (self.accumulateTime >= self.period) then
                self.funcHandle()
                self.haveRunTimes = self.haveRunTimes + 1
                self.accumulateTime = 0
            end
            -- 如果是0 则无限循环
            if (self.runTimes ~= 0 and self.haveRunTimes >= self.runTimes) then
                self.bFinished = true
            end
        end
    end
end

---@class FrameTask
local FrameTask = class('FrameTask', TimesRelatedTask)

function FrameTask:ctor(delay, period, runTimes, funcHandle)
    self.super.ctor(self, delay, period, runTimes, funcHandle, true)
    self.frameCount = 0
end

function FrameTask:Tick(deltaTime)
    if (self.bFinished and self.bPaused) then
        return
    end
    -- 如果是间隔数帧调用一次，可以获取数帧总时间
    self.accumulateTime = self.accumulateTime + deltaTime
    self.frameCount = self.frameCount + 1
    if (not self.firstRun) then
        if (self.frameCount >= self.delay) then
            self.funcHandle(self.accumulateTime)
            self.haveRunTimes = 1
            self.firstRun = true
            self.accumulateTime = 0
        end
    else
        -- 周期执行
        if (self.period <= 0) then
            self.bFinished = true
        else
            if (self.frameCount >= self.period) then
                self.funcHandle(self.accumulateTime)
                self.haveRunTimes = self.haveRunTimes + 1
                self.frameCount = 0
                self.accumulateTime = 0
            end
            -- 如果是0 则无限循环
            if (self.runTimes ~= 0 and self.haveRunTimes >= self.runTimes) then
                self.bFinished = true
            end
        end
    end
end

---@class TimerMgr
local TimerMgr = class('TimerMgr')

function TimerMgr:ctor(tickHandler)
    self.taskList = {}
    tickHandler:Add(self.Tick, self)
end

function TimerMgr:Tick(deltaTime)
    for i = #self.taskList, 1, -1 do
        local task = self.taskList[i]
        if (task:IsFinished()) then
            table.remove(self.taskList, i)
        elseif (task:IsPaused()) then
            
        else
            task:Tick(deltaTime)
        end
    end
end

function TimerMgr:AddTimeTask(delayTime, periodTime, maxRunTimes, funcHandle)
    local task = TimesRelatedTask.new(delayTime, periodTime, maxRunTimes, funcHandle)
    table.insert(self.taskList, task)
    return task
end

function TimerMgr:RunNextFrame(funcHandle)
    local task = FrameTask.new(1, 0, 1, funcHandle)
    table.insert(self.taskList, task)
    return task
end

function TimerMgr:AddFrameTask(delayFrame, periodFrame, maxRunTimes, funcHandle)
    local task = FrameTask.new(delayFrame, periodFrame, maxRunTimes, funcHandle)
    table.insert(self.taskList, task)
    return task
end




-- 用于tick
local g_tick_queue = {}

local function AddToTickQueue(funcSlot)
    for _, v in ipairs(g_tick_queue) do
        if (v == funcSlot) then
            LogError('tick', 'have added to tick queue %s', tostring(funcSlot))
            return
        end
    end
    table.insert(g_tick_queue, funcSlot)
end

local function TickGlobal(deltaTime)
    for _, v in ipairs(g_tick_queue) do
        v(deltaTime)
    end
end

_G.tickHandle = LuaEvent.new()
_G.TimerMgr = TimerMgr.new(_G.tickHandle)
local tickSlot = FuncSlot(function(...)
    _G.tickHandle(...)
end)
AddToTickQueue(tickSlot)

_G.TickGlobal = TickGlobal
