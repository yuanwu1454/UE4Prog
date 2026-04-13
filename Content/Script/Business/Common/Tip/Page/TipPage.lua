local TipPage = class("TipPage",PureMVC.GamePage);

local TipMediator = require("Business/Common/Tip/Mediator/TipMediator")

-- 最多展示的item数量
local MaxCacheNum = 3
-- 缓冲池数量
local PoolCacheNum = MaxCacheNum + 1
-- 每个item的最高展示时间
local SuvialTime = 1.2
-- 当个item的间隔
local DistInterval = 90

function TipPage:Construct()
    _G.bShowTipPage = true
    PureMVC.GamePage.Construct(self)
    self.cacheMsg = {}
end

function TipPage:Destruct()
    self.cacheMsg = {}
    _G.bShowTipPage = false
    PureMVC.GamePage.Destruct(self)
end

function TipPage:OnInitialized()
    TipPage.super.OnInitialized(self)
    self.cacheMsg = {}
    self:CreateCachePool()
end

function TipPage:ListNeededMediators()
    return {
        TipMediator,
    }
end

function TipPage:OnOpen(luaOpenData,nativeOpenData)
    LogInfo("TipPage", "Open TipPage")
    self:ShowMsg(luaOpenData.realMsg, luaOpenData.extra)
end

-- 消息队列缓冲
function TipPage:PushMsgData(msgDt)
    self.cacheMsg = self.cacheMsg or {}
    self.cacheMsg[#self.cacheMsg + 1] = msgDt
end

function TipPage:PopMsgData()
    self.cacheMsg = self.cacheMsg or {}
    return table.remove(self.cacheMsg, 1)
end

function TipPage:CheckHasMsg()
    self.cacheMsg = self.cacheMsg or {}
    return #self.cacheMsg > 0
end

local createMsgDt = function (msg, bPositive)
    return {
        message = msg,
        bPositive = bPositive
    }
end


function TipPage:ShowMsg(msg, oriData)
    local bPositive = true
    if type(oriData) == "number" and oriData ~= 0  then
        bPositive = false
    end

    local dt = createMsgDt(msg, bPositive)
    if type(oriData) == "table" and oriData.Pos == "Up" then
        dt.DiffDistance = -275
    end

    self:PushMsgData(dt)
    self:CreatePopTimer()
end

function TipPage:AddNewItem()
    local msgDt = self:PopMsgData()

    local FreeWidgetDt = self:GetFreeWidgetDt()
    local item = FreeWidgetDt.widget
    local slot = FreeWidgetDt.slot
    item:SetVisibility(UE4.ESlateVisibility.SelfHitTestInvisible)
    item:InitView({msg = msgDt.message})


    -- 修正y位置
    local dist = DistInterval

    self.DiffDistance = 0
    if msgDt.DiffDistance then
        self.DiffDistance = msgDt.DiffDistance
    end

    local y = self.DiffDistance
    if #self.children > 0 then
        y = self.children[#self.children].widget.Slot:GetPosition().Y + dist
    end

    slot:SetPosition(UE4.FVector2D(0,y))
    self.children[#self.children + 1] = {widget = item, time = 0, showAni = false}
    item:SetRenderOpacity(0)

    -- if msgDt.bPositive then
    --     self:K2_PostAkEvent(self.PositiveAudio)
    -- else
    --     self:K2_PostAkEvent(self.NegativeAudio)
    -- end
end

-- 当前最顶部的item的位置
function TipPage:GetIntervalDist()
    return (#self.children * DistInterval) + math.abs(self.DiffDistance)
end

function TipPage:CreatePopTimer()
    if self.animOpenPageTask == nil then
        self.children = {}
        self:AddNewItem()
        -- 每次移动的距离
        local dist = 15
        -- 时间间隔
        local intervalTime = 0.01

        self.animOpenPageTask = TimerMgr:AddTimeTask(0, intervalTime, 0, function()
            -- 1. 检测是否需要移动当前位置
            if self:CheckHasMsg() or (self.children[1] and self.children[1].widget.Slot:GetPosition().Y > -self:GetIntervalDist()) then
                for i, v in ipairs(self.children) do
                    local y = v.widget.Slot:GetPosition().Y
                    v.widget.Slot:SetPosition(UE4.FVector2D(0, y - dist))
                end
            end
            -- 2. 更新当前每个item的时间
            for i, v in ipairs(self.children) do
                v.time = v.time + intervalTime
            end

            -- 3. 设置透明度
            for i, v in ipairs(self.children) do
                local x = v.time
                local opa 
                -- 展示动画
                if v.time < SuvialTime/3 then
                    -- opa = v.time / (SuvialTime/3)
                    if v.showAni == false then
                        v.showAni = true
                        v.widget:PlayShowAni()
                    end
                    opa = 1
                -- 不变
                elseif v.time < SuvialTime*2/3 then
                    opa = 1
                -- 渐隐
                else
                    opa = (SuvialTime - v.time) / (SuvialTime/3)
                end
                v.widget:SetRenderOpacity(opa)
            end

            -- 4. 检测是否有新的消息并且最上面的item 已经到顶了
            if (self.children[1] and self.children[1].widget.Slot:GetPosition().Y <= -self:GetIntervalDist()) and self:CheckHasMsg() then
                self:AddNewItem()
            end

            -- 5. 移除不需要的item
            if #self.children > MaxCacheNum or (self.children[1] and self.children[1].time > SuvialTime) then
                -- 隐藏即可， 不需要删除
                self.children[1].widget:SetVisibility(UE4.ESlateVisibility.Collapsed)

                table.remove(self.children, 1)
            end

            -- 6. 如果没有需要展示的item， 就clear 掉 timer 
            if #self.children == 0 then
                self:DestoryPopTimer()
            end
        end)
    end
end

function TipPage:DestoryPopTimer()
    if self.animOpenPageTask then
        self.animOpenPageTask:EndTask()
        self.animOpenPageTask = nil
    end
end

function TipPage:CreateCachePool()
    self.pool = {}
    local itemClass = ObjectUtil.LoadClass(self.itemClass)
    -- 最多展示的item数量
    for i = 1, PoolCacheNum do
        local item = UE4.UWidgetBlueprintLibrary.Create(self, itemClass)
        local slot = self.CanvasPanel_Center:AddChild(item)
        item:SetRenderOpacity(0)
        item:SetVisibility(UE4.ESlateVisibility.Collapsed)
        self.pool[#self.pool+1] = { widget = item,slot = slot}
    end
end

function TipPage:GetFreeWidgetDt()
    if self.freeIndex == nil then
        self.freeIndex = 1
    else
        self.freeIndex = ((self.freeIndex + 1) % PoolCacheNum)
        if self.freeIndex == 0 then
            self.freeIndex = PoolCacheNum
        end
    end
    return self.pool[self.freeIndex]
end


function TipPage:OnClose()
    LogInfo("TipPage", "OnClose TipPage")
      _G.bShowTipPage = false
    self:DestoryPopTimer()
end

return TipPage