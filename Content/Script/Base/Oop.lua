--[[
@author PureMVC LUA Native Port by Ravior 
@author Copyright(c) 2015 Gitlib, Inc., Some rights reserved.
 
@hide
A an internal helper class used to assist classlet implementation. This
class is not accessible by client code.
]]

-- 模拟class继承
-- 类继承由于调用少，可以deep copy
-- 类实例调用多，用setmetatable
local function clone(other)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for key, value in pairs(object) do
            new_table[_copy(key)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(other)
end

local classMap = {}

function class(classname, super)
	local superType = type(super)
	local cls
	if superType ~= 'function' and superType ~= 'table' then
		superType = nil
		super = nil
	end

	if superType == 'function' or (super and super.__ctype == 1) then
		-- inherited from native C++ Object
		cls = {}
		if superType == 'table' then
			-- copy fields from super
			for k,v in pairs(super) do cls[k] = v end
			cls.__create = super.__create
			cls.super = super
		else
			cls.__create = super
		end
		cls.ctor = function() end
		cls.__cname = classname
		cls.__ctype = 1

		function cls.new(...)
			local instance = cls.__create(...)
			-- copy fields from class to  native object
			for k, v in pairs(cls) do instance[k] = v end
			instance.class = cls
			instance:ctor(...)
			return instance
		end
	else
		-- inherited from Lua Object
		if super then
            cls = clone(super)
            cls.super = super
		else
			cls = {ctor = function() end}
		end
		cls.__cname = classname
		cls.__ctype = 2 --lua
        if not cls.disable_index_set then -- 兼容特殊逻辑
            cls.__index = cls
        end

		function cls.new(...)
			local instance = setmetatable({}, cls)
			instance.class = cls
			instance:ctor(...)
			return instance
		end
	end

	-- 🔥 全局统一添加 GetName 方法（所有类都会拥有）
	function cls:GetName()
		return tostring(self.__cname)
	end
	classMap[classname] = cls
	return cls
end

_G.class = class
_G.clone = clone
_G.getClass = function (classname)
    return classMap[classname]
end