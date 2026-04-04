local repeatCheckTbl = {}
local parentName = "CommandDefines"
local meta
meta = {
	__newindex = function(t, key, value)
		if value == nil then
			rawset(t, key, value)
			return 
		end
        if t[key] ~= nil then
			LogError("CommandDefines", "error repeat key " ..  tostring(key) .. " will be overwrite")
            return
        end

        if type(value) == "table" then
			setmetatable(value, meta)
			rawset(value, "__parent", t)
            rawset(value, "__parentName", key)
            rawset(t, key, value)

			for k, v in pairs(value) do
				if k ~= "__parent" and k ~= "__parentName" then
					value[k] = nil
					value[k] = v
				end
			end

        else
			if repeatCheckTbl[value] == true then
                local str = tostring(key)
                while t.__parent do
                    if str == "" then
                        str = t.__parentName
                    else
                        str = t.__parentName .. "." .. str
                    end
                    t = t.__parent
                end
                str = parentName .. "." .. str
				LogError("CommandDefines", "repeat value: key ".. tostring(str) .. " value " .. tostring(value))
                return
			end
			repeatCheckTbl[value] = true
			if BUILD_DEVELOPMENT or BUILD_DEBUG then
				local str = key
				local st = t
				while st.__parent do
					if str == "" then
						str = st.__parentName
					else
						str = st.__parentName .. "." .. str
					end
					st = st.__parent
				end
				str = parentName .. "." .. str	
				LogInfo("CommandDefines", " Key is ".. tostring(str) .. " Value is " .. tostring(value))
			end

			rawset(t, key, value)
		end
	end
}

local CommandDefines = {}
setmetatable(CommandDefines, meta)
_G.CD = CommandDefines