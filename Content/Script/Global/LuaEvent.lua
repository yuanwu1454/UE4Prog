
require("Global.FuncSlot")
---@class LuaEvent
---@field new fun():LuaEvent
local LuaEvent = class('LuaEvent')

function LuaEvent:ctor()
    self.multiBroadcastEvent = {} ---@type FuncSlot[]
end

---添加事件回调
---@param func fun
---@param obj any
function LuaEvent:Add(func, obj)
    local slot = FuncSlot(func, obj)
    return self:AddFuncSlot(slot)
end

---添加事件回调
---@param slot FuncSlot
function LuaEvent:AddFuncSlot(slot)
	for _,v in ipairs(self.multiBroadcastEvent) do
		if v == slot then
			LogWarn('LuaEvent', 'FuncSlot(func:%s obj:%s) have added to lua event', tostring(slot.func), tostring(slot.obj))
			return
		end
	end
	table.insert(self.multiBroadcastEvent, slot)
end

---添加一次性的事件回调，触发一次后移除
---@param func fun
---@param obj any
function LuaEvent:AddOnce(func, obj)
	local slot = FuncSlot(func, obj)
	return self:AddFuncSlotOnce(slot)
end

---添加一次性的事件回调，触发一次后移除
---@param slot FuncSlot
function LuaEvent:AddFuncSlotOnce(slot)
	slot._isOnce = true
	return self:AddFuncSlot(slot)
end

function LuaEvent:Remove(func, obj)
    local slot = FuncSlot(func, obj)
    return self:RemoveFuncSlot(slot)
end

function LuaEvent:RemoveFuncSlot(slot)
	for i = #self.multiBroadcastEvent, 1, -1 do
		local tmpSlot = self.multiBroadcastEvent[i]
		if tmpSlot == slot then
			table.remove(self.multiBroadcastEvent, i)
		end
	end
end

function LuaEvent:IsEmpty()
	return #self.multiBroadcastEvent == 0
end

function LuaEvent:__call(...)
	if self:IsEmpty() then 
		return 
	end
	for i = #self.multiBroadcastEvent, 1, -1 do
		local v = self.multiBroadcastEvent[i]
		if v._isOnce then
			table.remove(self.multiBroadcastEvent, i)
		end
		v(...)
	end
end

_G.LuaEvent =  LuaEvent