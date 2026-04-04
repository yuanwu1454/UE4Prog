--------------------------------------------------------------------------------
-- FuncSlot : 函数槽，基于ToLua的Slot，我们改下名字叫FuncSlot更明确一些
-- 注意传参顺序,如果不需要传obj，调用是会是func(...)，否则是func(obj, ...)
--------------------------------------------------------------------------------
local setmetatable = setmetatable

local _func_slot = {}
setmetatable(_func_slot, _func_slot)

_func_slot.__call = function(self, ...)
	if not self.func then
		LogError("FuncSlot", "func is nil")
		return
	end
	if nil == self.obj then
		return self.func(...)
    else
		return self.func(self.obj, ...)
	end
end

_func_slot.__eq = function (lhs, rhs)
	return lhs.func == rhs.func and lhs.obj == rhs.obj
end

--可用于 Timer 定时器回调函数. 例如Timer.New(slot(self.func, self))
local function FuncSlot(func, obj)
	return setmetatable({func = func, obj = obj}, _func_slot)
end

_G.FuncSlot = FuncSlot