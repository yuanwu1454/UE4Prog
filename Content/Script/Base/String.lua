local string = _G.string

function string.IsNilOrEmpty(str)
	return str == nil or str == ""
end

function string.safesplit(splitstr, sep)
	splitstr = tostring(splitstr)
    sep = tostring(sep)
    if (sep=='') then return false end
    local pos, arr = 0, {}
    for st,sp in function() return string.find(splitstr, sep, pos, true) end do
        table.insert(arr, string.sub(splitstr, pos, st - 1))
        pos = sp + 1
    end
    table.insert(arr, string.sub(splitstr, pos))
    return arr
end

function string.split(splitstr, sep)
	if splitstr and #splitstr > 0 then
		local b, ret = pcall(string.safesplit, splitstr, sep)
		if b then
			return ret
		else
			printerror("splitstr:", splitstr, "| sep:", sep, "| errmsg:", ret)
			return {}
		end
	end
	return {}
end

-- 进行二次分隔。
-- 例：string.splitSecondary("00:00:07,000", ":", ",")
-- 最终拆分为{"00","00","07","000"}
function string.splitSecondary(splitstr, first, secondary)
	if splitstr and #splitstr > 0 then
		local b, retOne = pcall(string.safesplit, splitstr, first)
		local res = {}
		if b then
			for _, strOne in ipairs(retOne) do
				local c, retTwo = pcall(string.safesplit, strOne, secondary)
				for _, strTwo in ipairs(retTwo) do
					table.insert(res,strTwo)
				end
				if #retTwo == 0 then
					table.insert(res,retOne)
				end
			end

			return res
		else
			printerror("splitstr:", splitstr, "| sep:", first, "| errmsg:", retOne)
			return {}
		end
	end
	return {}
end

-- 废弃，当这种情况下无法正确切分："|aa|bb||dd"  分割符："|"
-- 正常情况下返回：{[1] = "", [2] = "aa", [3] = "bb", [4] = "", [5] = "dd"}
-- 目前返回：{[1] = "aa", [2] = "bb", [3] = "dd"}
-- function string.split(splitstr, sep)
-- 	if sep == nil then
-- 		sep = "%s"
-- 	end
-- 	local t = {}
-- 	local i = 1
-- 	for str in string.gmatch(splitstr, "([^"..sep.."]+)") do
-- 		t[i] = str
-- 		i = i + 1
-- 	end
-- 	return t
-- end

string.oriformat = string.format
function string.format(s, ...)
	local list = {}
	local len = select("#", ...)
	for i=1, len do
		local v = select(i, ...)
		if v == nil or type(v) == "boolean" then
			table.insert(list, tostring(v))
		else
			table.insert(list, v)
		end
	end
	return string.oriformat(s, table.unpack(list))
end

function string.startswith(s, starts)
	if #starts > #s then
		return false
	end
	for i = 1, #starts do
		if string.byte(s, i) ~= string.byte(starts, i) then
			return false
		end
	end
	return true
end

function string.endswith(s, ends)
	local lenS = #s
	local lenEnds = #ends
	if lenEnds > lenS then
		return false
	end
	local offset = lenS - lenEnds
	for i = 1, lenEnds do
		if string.byte(s, offset+i) ~= string.byte(ends, i) then
			return false
		end
	end
	return true
end

--非正则替换
function string.replace(s, pat, repl, n)
	local list = {"(", ")", ".", "+", "-", "*", "?", "[", "^", "$"}
	for k, v in ipairs(list) do
		pat = string.gsub(pat, "%"..v, "%%"..v)
	end
	return string.gsub(s, pat, repl, n)
end

--string.eval("a+b", {a=1, b=2})
function string.eval(s, t)
	local f = loadstring(string.format("do return %s end", s))
	setfenv(f, t)
	return f()
end

--获取固定长度字符串，超出长度都用……替代
function string.gettitle(str, size, sPattern)
	local sPattern = sPattern or "……"
	local t = string.getutftable(str)
	local result = {}
	local cnt = 0
	for k, v in pairs(t) do
		if string.byte(v) > 0xc0 then
			cnt = cnt + 2
		else
			cnt = cnt + 1
		end
		if cnt <= size then
			table.insert(result, v)
		else
			table.insert(result, sPattern)
			break
		end
	end
	return table.concat(result, "")
end

-- 获取指定长度随机字符串 onlynum(仅数字)
function string.GetRandomString(iLen, onlynum)
	local function random(n, m)
		math.randomseed(os.clock()*math.random(1000000, 90000000)+math.random(1000000, 90000000))
		return math.random(n, m)
	end
	local function randomString(len)
		local bc = "QWERTYUIOPASDFGHJKLZXCVBNM"
		local sc = "qwertyuiopasdfghjklzxcvbnm"
		local no = "0123456789"
		local tmplete = onlynum and no or no .. sc .. bc
		local maxLen = #tmplete

		local srt = {}
		for i=1,len,1 do
			local index = random(1, maxLen)
			srt[i] = string.sub(tmplete, index, index)
		end
		return table.concat(srt, "")
	end
	return randomString(iLen)
end

-- 通配符替换
-- infoStr：要替换的字符串
-- wildcard：替换的正则表达式，为nil时使用 "A%d+A"
-- func：传入查找某个配置表的方法
function string.WildcardReplace(infoStr, id, func, wildcard)
	wildcard = wildcard or "A%d+A"

	if id == nil and func == nil then
		LogDebug('[string]',string.format("//Error: 通配符函数传入的参数有误 id = %s , func = %s ",id, func))
		return "null"
	end

	-- 使用默认的方法
	if id ~= nil and func == nil then
		func = function(word) return CConfigUtils.GetLevelValueById(id,  word) end
	end

	local replaceTable = {}
	for word in string.gmatch(infoStr, wildcard) do
		replaceTable[#replaceTable +1] = {word, func(word)}
	end

	for _, T in ipairs(replaceTable) do
		if T[2] == nil then
			LogDebug('[string]',string.format("//Error: 找不到对应通配符的值 -->> %s (%s) id = %d ",infoStr,T[1],id))
			return "null"
		end
		infoStr = string.gsub(infoStr,T[1],T[2])
	end

	return infoStr
end

-- 字符串换行分割与颜色搭配
function string.StringWrapSplit(OriginalStr, color)

	local strColor = ""
	local firstLine = true
	local tempStr = ""
	local strSplit = string.split(OriginalStr,"\n")

	if table.count(strSplit) > 1 then
		for _, str in ipairs(strSplit) do
			if firstLine then
				if color then
					tempStr = string.format("<span color=\"#%s\">%s</>",color,str)
				else
					tempStr = string.format("%s",str)
				end
				firstLine = false
			else
				if color then
					tempStr = string.format("\n<span color=\"#%s\">%s</>",color,str)
				else
					tempStr = string.format("\n%s",str)
				end
			end
			strColor = strColor..tempStr
		end

	elseif table.count(strSplit) == 1 then
		if color then
			strColor = string.format("<span color=\"#%s\">%s</>",color,OriginalStr)
		else
			strColor = string.format("%s",OriginalStr)
		end
	end

	return strColor
end

-- 去掉字符串中的[[xxx]]里的内容，包括中括号自身
function string.RemoveParenthesis(OriginalStr)
	OriginalStr = string.gsub(OriginalStr, "%[%[", "<")       -- 要替换特殊的字符串，要在特殊的字符串前面加%
	OriginalStr = string.gsub(OriginalStr, "%]%]", ">")
	OriginalStr = string.gsub(OriginalStr, "<.->", "")
	return OriginalStr
end

-- bytes转string
function string.CovertBytesToString(t)
    local bytearr = {}
    for k, v in pairs(t) do
        local utf8byte = v < 0 and (0xff + v + 1) or v
        local cc = string.char(utf8byte)
        table.insert(bytearr, cc)
    end
    return tostring(table.concat(bytearr))
end

function string.ByteToMB(num)
	local d = 1024^2
	local l = num * 100
	if l < d then
		return 0.01
	end
	local mb = num / d
	return mb - mb%0.01
end

--移除字符串中空格
function string.trim(url)
	return string.gsub(url, "%s+", "")
end

return string