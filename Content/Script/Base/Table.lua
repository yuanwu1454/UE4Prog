
local table = _G.table
function table.print(t, msg)
    if (_G.NO_LOGGING) then
        return
    end
    -- if true then
    --     -- print("return table.print")
    --     return 
    -- end
    -- 用于表注各个模块对应的记录 尤其时table 里面的记录
    local print = function (...)
        if msg then
            print(msg, ...)
        else
            print(...)
        end
    end

    local callerData = debug.getinfo(2, "n")
    print("[lua]table.print from ", callerData.name, callerData.namewhat)
    local print_cache = {}
    local function sub_print_r(t, indent)
        if (print_cache[tostring(t)]) then
            print(indent.."*"..tostring(t))
        else
            print_cache[tostring(t)] = true
            if (type(t) == "table") then
                for pos,val in pairs(t) do
                    if (type(val) == "table") then
                        print(indent.."["..pos.."] => "..tostring(t).." {")
                        sub_print_r(val,indent..string.rep(" ",string.len(pos)+8))
                        print(indent..string.rep(" ",string.len(pos)+6).."}")
                    elseif (type(val) == "string") then
                        print(indent.."["..pos..'] => "'..val..'"')
                    else
                        print(indent.."["..pos.."] => "..tostring(val))
                    end
                end
            else
                print(indent..tostring(t))
            end
        end
    end
    if (type(t) == "table") then
        print(tostring(t).." {")
        sub_print_r(t, "  ")
        print("}")
    else
        sub_print_r(t, "  ")
    end
    print()
end

function table.count(t)
    local count = 0
    for k, v in pairs(t) do
        count = count + 1
    end
    return count
end

function table.index(t, element)
    for k, value in pairs(t or {}) do
        if value == element then
            return k
        end
    end
end

function table.removeElement(t, element)
	local idx = table.index(t, element)
	if idx then
		return table.remove(t, idx)
	end
end

---@generic T
---@param t T[]|table<any,T>
---@param matchFunc fun(elem:T):boolean
---@return T 返回首个符合条件的元素，找不到返回nil
function table.find(t, matchFunc)
	if not t or not matchFunc then return end
	for k, value in pairs(t) do
		if matchFunc(value) then
			return value
		end
	end
end

function table.keys(t)
    local keys = {}
    for k, v in pairs(t) do
        table.insert(keys, k)
    end
    return keys
end

function table.values(t)
    local values = {}
    for k, v in pairs(t) do
        table.insert(values, v)
    end
    return values
end

function table.extend(t1, t2)
    for k, v in ipairs(t2) do
        table.insert(t1, v)
    end
    return t1
end

function table.merge(t1, t2)
    for k, v in pairs(t2) do
        t1[k] = v
    end
    return t1
end

function table.equal(t1, t2)
    if t1 == t2 then
        return true
    end
    if type(t1) == "table" and type(t2) == "table" then
        if t1.GetInstanceID and t2.GetInstanceID then
            return t1:GetInstanceID() == t2:GetInstanceID()
        end
        if table.count(t1) ~= table.count(t2) then
            return false
        end
        for k, v in pairs(t1) do
            if not table.equal(v, t2[k]) then
                return false
            end
        end
        return true
    end
    return false
end

function table.copy(src, dst)
    local tableDict = {}
    local function process(src, dst)
        local function value(o)
            if type(o) == "table" then
                if tableDict[o] then
                    -- print("copy self !!!")
                    return {}
                end
                tableDict[o] = true
                local tbl = {}
                for k, v in pairs(o) do
                    tbl[k] = process(v)
                end
                return tbl
            else
                return o
            end
        end
        if src == nil then
            printerror("table.copy src is nil")
            return dst
        end 
        if dst then
            for k, v in pairs(value(src)) do
                dst[k] = v
            end
            return dst
        else
            return value(src)
        end
    end

    return process(src, dst)
end

function table.copyproto(src)
    local dst = table.copy(src)
    dst = setmetatable(dst, {
        __index = function(t, k)
            local v = rawget(t, k)
            if not v then
                return src[k]
            end
        end
    })

    return dst
end

-- 包含metatable __index
function table.copyfullproto(src, dst)
    local tableDict = {}
    local function process(src, dst)
        local function value(o)
            if type(o) == "table" then
                if tableDict[o] then
                    -- print("copy self !!!")
                    return {}
                end
                tableDict[o] = true
                local tbl = {}
                local mt = getmetatable(o)
                if mt and mt.__index then
                    tbl = setmetatable(tbl, {__index = mt.__index})
                end
                for k, v in pairs(o) do
                    tbl[k] = process(v)
                end
                return tbl
            else
                return o
            end
        end
        if src == nil then
            printerror("table.copy src is nil")
            return dst
        end 
        if dst then
            for k, v in pairs(value(src)) do
                dst[k] = v
            end
            return dst
        else
            return value(src)
        end
    end

    return process(src, dst)
end




--  打印 table
--  t           table
-- name         table名称
-- save         是否存储数据到log
-- log2console  是否显示到console
-- maxlayer     table最大层数
-- strfix       添加前后词缀，用于log中括起数据段，易于识别
function table.show(t, name, save, savename, log2console, maxlayer, strfix)
    local function table_tostring(t, maxlayer, name)
        local tableDict = {}
        local layer = 0
        maxlayer = maxlayer or 999
        local function cmp(t1, t2)
            if type(t1) == "number" and type(t1) == type(t2) then
                return t1 < t2
            end
            return tostring(t1) < tostring(t2)
        end
        local function table_r (t, name, indent, full, layer)
            local id = not full and name or type(name)~="number" and tostring(name) or '['..name..']'
            local tag = indent .. id .. ' = '
            if string.len(tag) > 10000 then
                error("############### log long 1000")
                return table.concat(out, '\n')
            end

            local out = {}  -- result
            if type(t) == "table" and layer < maxlayer then
                if tableDict[t] ~= nil then
                    table.insert(out, tag .. '{} -- ' .. tableDict[t] .. ' (self reference)')
                else
                    tableDict[t] = full and (full .. '.' .. id) or id
                    if next(t) then -- Table not empty
                        table.insert(out, tag .. '{')
                        local keys = {}
                        for key,value in pairs(t) do
                            table.insert(keys, key)
                        end
                        table.sort(keys, cmp)
                        for i, key in ipairs(keys) do
                            local value = t[key]
                            table.insert(out,table_r(value,key,indent .. '    ',tableDict[t], layer + 1))
                        end
                        table.insert(out,indent .. '},')
                    else
                        table.insert(out,tag .. '{},')
                    end
                end
            else
                local val = type(t)~="number" and type(t)~="boolean" and '"'..tostring(t)..'"' or tostring(t)
                table.insert(out, tag .. val .. ",")
            end
            return table.concat(out, '\n')
        end
        return table_r(t, name or 'Error:请打印调用来源,方便他人阅读.Table', '', '', layer)--//-code parse
    end

    local s = table_tostring(t, maxlayer, name)
    s = string.sub(s, 1, -2)
    -- local s = table_tostring(t, maxlayer, name), debug.traceback()
    if save then
        local filename = savename or "debugtable.lua"
        local path = UE.UBlueprintPathsLibrary.ProjectSavedDir()
        local fullname = path .. filename
        local file = io.open(fullname, "a+")
        if file then
            print(string.format("已保存数据到指定文件: %s: %s -> %s", name, s, file)) --//-code parse
            file:write("\n" .. s)
            file:close()
        else
            print("文件写入失败")--//-code parse
        end
    end

    if log2console ~= false then
        if strfix ~= false then
            print("===== 以下是 table.show 输出数据 =====\n" .. s .. "\n----- 以上是 table.show 输出数据 -----")--//-code parse
        else
            print(s)
        end
    end
end

--[[
deep clone lua table
--]]
---@generic T
---@param object T
---@return T
function table.clone(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local newObject = {}
        lookup_table[object] = newObject
        for key, value in pairs(object) do
            newObject[_copy(key)] = _copy(value)
        end
        return setmetatable(newObject, getmetatable(object))
    end
    return _copy(object)
end

---@return boolean
function table.containsValue(t, Value)
    if t == nil then
        LogError("table.containsValue", "param talble is nil!")
        return false
    end
    for key, value in pairs(t) do
        if tostring(value) == tostring(Value) then
            return true
        end
    end
   
    return false
end

--[[
    Binary Search
    t: search table
    Value: target value
    tValFunc: surppot complex table

    return: If value exist, return it`s pos, 
        otherwise return insert pos.
]]
function table.binarySearch(t, Value, tValFunc)
    local low = 1
    local high = #t
    local mid
    while(low <= high) do
        mid = math.ceil((low + high) / 2)
        local tVal = tValFunc and tValFunc(t[mid]) or t[mid]
        if (tVal == Value) then
            return mid
        elseif (tVal < Value) then
            low = mid + 1
        else
            high = mid - 1
        end
    end
    return mid
end

return table