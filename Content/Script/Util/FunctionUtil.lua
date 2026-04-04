---@class FunctionUtil
---@return FunctionUtil
local FunctionUtil = {}

local SECONDSFORDAY, SECONDSFORHOUR, SECONDSFORMINUTE = 86400, 3600, 60
local DaysText,
    DaysHoursText,
    HoursText,
    HoursMinutesText,
    HoursMinutesSecondsText,
    MinutesText,
    MinutesSecondsText,
    SecondsText

---@class LuaTimeSpan
---当传入UnixTimestamp为时长秒数时以下属性才可用
---@field Day number @总天数
---@field Hour number @剩余小时部分
---@field Hours number @剩余总小时数
---@field Minute number @剩余分钟部分
---@field Minutes number @剩余总分钟数
---@field Second number @剩余秒数部分
---@field Seconds number @剩余总秒数
---@field PMGameUtil_Format_Days string @根据ST_Common读取 d天
---@field PMGameUtil_Format_DaysHours string @根据ST_Common读取 d天h小时
---@field PMGameUtil_Format_Hours string @根据ST_Common读取 h小时
---@field PMGameUtil_Format_HoursMinutes string @根据ST_Common读取 h小时m分钟
---@field PMGameUtil_Format_Minutes string @根据ST_Common读取 m分钟
---@field PMGameUtil_Format_MinutesSeconds string @根据ST_Common读取 m分钟s秒
---@field PMGameUtil_Format_Seconds string @根据ST_Common读取 s秒
---@field PMGameUtil_Format_HoursMinutesSeconds string @根据ST_Common读取 hh:mm:ss
---@field PMGameUtil_Format_HoursMinutesShort string @根据ST_Common读取 hh:mm
---@field PMGameUtil_Format_MinutesSecondsShort string @根据ST_Common读取 mm:ss
---@field PMGameUtil_Format_ExpectUnit string @仅读取天,小时,分钟,秒 保留一个单位
---@field PMGameUtil_Format_ExpectTwoUnit string @仅读取[d天h小时],[h小时m分钟],[m分钟s秒],[s秒(<1min)] 最多保留两个单位
---当UnixTimestamp为UTC0时区Unix时间戳时以下属性才可使用
---@field PMGameUtil_Format_Date string 返回本地时区的日期格式 yyyy年mm月dd日
---@field PMGameUtil_Format_MonthDay string 返回本地时区的日期格式 mm月dd日
---@field PMGameUtil_Format_DateShort string 返回本地时区的简短日期格式 yyyy/mm/dd
---@field PMGameUtil_Format_DateShorter string 返回本地时区的日期格式(不包含年份) mm/dd
---@field PMGameUtil_Format_Time string 返回本地时区的时间格式 HH:MM:SS
---@field PMGameUtil_Format_TimeShort string 返回本地时区的时间格式（省略秒） HH:MM
---@field PMGameUtil_Format_UTCDate string 返回UTC时区的日期格式 yyyy年mm月dd日
---@field PMGameUtil_Format_UTCDateShort string 返回UTC时区的简短日期格式 yyyy/mm/dd
---@field PMGameUtil_Format_UTCTime string 返回UTC时区的时间格式 HH:MM:SS
---@field PMGameUtil_Format_UTCTimeShort string 返回UTC时区的时间格式（省略秒） HH:MM

local LuaTimeSpanMeta = {}
LuaTimeSpanMeta.getters = {}
function LuaTimeSpanMeta.__eq(lhs, rhs)
    return lhs.UnixTimestamp == rhs.UnixTimestamp
end
---@return number 剩余总天数
function LuaTimeSpanMeta.getters.Day(self, key)
    local TempDays = math.modf(self.UnixTimestamp / SECONDSFORDAY)
    TempDays = TempDays < 0 and 0 or TempDays
    rawset(self, key, TempDays) --将值缓存进对象，下次无需计算直接取
    return TempDays
end
function LuaTimeSpanMeta.getters.Days(self, key)
    return self.Day
end
---@return number 剩余总小时
function LuaTimeSpanMeta.getters.Hours(self, key)
    local TempHours = math.modf(self.UnixTimestamp / SECONDSFORHOUR)
    TempHours = TempHours < 0 and 0 or TempHours
    rawset(self, key, TempHours)
    return TempHours
end
---@return number 剩余小时部分
function LuaTimeSpanMeta.getters.Hour(self, key)
    local TempHour = self.Hours - self.Day * 24 --总小时数减去总天数*24时 算出剩余小时
    TempHour = TempHour < 0 and 0 or TempHour
    rawset(self, key, TempHour)
    return TempHour
end
---@return number 剩余总分钟数
function LuaTimeSpanMeta.getters.Minutes(self, key)
    local TempMinutes = math.modf(self.UnixTimestamp / SECONDSFORMINUTE)
    TempMinutes = TempMinutes < 0 and 0 or TempMinutes
    rawset(self, key, TempMinutes)
    return TempMinutes
end
---@return number 剩余分钟部分
function LuaTimeSpanMeta.getters.Minute(self, key)
    local TempMinute = self.Minutes - self.Hours * 60 --总分钟数减去总小时*60分 算出剩余分
    TempMinute = TempMinute < 0 and 0 or TempMinute
    rawset(self, key, TempMinute)
    return TempMinute
end
---@return number 剩余总秒数
function LuaTimeSpanMeta.getters.Seconds(self, key)
    local TempSeconds = math.modf(self.UnixTimestamp)
    TempSeconds = TempSeconds < 0 and 0 or TempSeconds
    rawset(self, key, TempSeconds)
    return TempSeconds
end
---@return number 剩余秒数部分
function LuaTimeSpanMeta.getters.Second(self, key)
    local TempSecond = self.Seconds - self.Minutes * 60 --总秒数减去总分钟*60秒 算出剩余秒
    TempSecond = TempSecond < 0 and 0 or TempSecond
    rawset(self, key, TempSecond)
    return TempSecond
end
---@return string 返回格式：d天
function LuaTimeSpanMeta.getters.PMGameUtil_Format_Days(self, key)
    DaysText = DaysText or ConfigMgr:FromStringTable(StringTablePath.ST_Common, "PMGameUtil_Format_DaysHours")
    local InPMGameUtil_Format_Days = ObjectUtil:GetTextFromFormat(DaysText, {Days = self.Day})
    rawset(self, key, InPMGameUtil_Format_Days) --将字符串缓存进对象，下次无需再生成直接取
    return InPMGameUtil_Format_Days
end
---@return string 返回格式：d天h小时
function LuaTimeSpanMeta.getters.PMGameUtil_Format_DaysHours(self, key)
    DaysHoursText =
        DaysHoursText or ConfigMgr:FromStringTable(StringTablePath.ST_Common, "PMGameUtil_Format_DaysHours1")
    local InPMGameUtil_Format_DaysHours =
        ObjectUtil:GetTextFromFormat(DaysHoursText, {Days = self.Day, Hours = self.Hour})
    rawset(self, key, InPMGameUtil_Format_DaysHours)
    return InPMGameUtil_Format_DaysHours
end
---@return string 返回格式：h小时
function LuaTimeSpanMeta.getters.PMGameUtil_Format_Hours(self, key)
    HoursText = HoursText or ConfigMgr:FromStringTable(StringTablePath.ST_Common, "PMGameUtil_Format_Hours")
    local InPMGameUtil_Format_Hours = ObjectUtil:GetTextFromFormat(HoursText, {Hours = self.Hours})
    rawset(self, key, InPMGameUtil_Format_Hours)
    return InPMGameUtil_Format_Hours
end
---@return string 返回格式：h小时m分钟
function LuaTimeSpanMeta.getters.PMGameUtil_Format_HoursMinutes(self, key)
    HoursMinutesText =
        HoursMinutesText or ConfigMgr:FromStringTable(StringTablePath.ST_Common, "PMGameUtil_Format_Hours1")
    local InPMGameUtil_Format_HoursMinutes =
        ObjectUtil:GetTextFromFormat(HoursMinutesText, {Hours = self.Hours, Minutes = self.Minute})
    rawset(self, key, InPMGameUtil_Format_HoursMinutes)
    return InPMGameUtil_Format_HoursMinutes
end
---@return string 返回格式：m分钟
function LuaTimeSpanMeta.getters.PMGameUtil_Format_Minutes(self, key)
    MinutesText = MinutesText or ConfigMgr:FromStringTable(StringTablePath.ST_Common, "PMGameUtil_Format_Minutes")
    local InPMGameUtil_Format_Minutes = ObjectUtil:GetTextFromFormat(MinutesText, {Minutes = self.Minutes})
    rawset(self, key, InPMGameUtil_Format_Minutes)
    return InPMGameUtil_Format_Minutes
end
---@return string 返回格式：m分钟s秒
function LuaTimeSpanMeta.getters.PMGameUtil_Format_MinutesSeconds(self, key)
    MinutesSecondsText =
        MinutesSecondsText or ConfigMgr:FromStringTable(StringTablePath.ST_Common, "PMGameUtil_Format_MinutesSeconds")
    local InPMGameUtil_Format_MinutesSeconds =
        ObjectUtil:GetTextFromFormat(MinutesSecondsText, {Minutes = self.Minutes, Seconds = self.Second})
    rawset(self, key, InPMGameUtil_Format_MinutesSeconds)
    return InPMGameUtil_Format_MinutesSeconds
end
---@return string 返回格式：hh:mm
function LuaTimeSpanMeta.getters.PMGameUtil_Format_HoursMinutesShort(self, key)
    local PMGameUtil_Format_HoursMinutesShort=string.format("%02d", self.Hour) .. ':' .. string.format("%02d", self.Minute)
    rawset(self, key, PMGameUtil_Format_HoursMinutesShort)
    return PMGameUtil_Format_HoursMinutesShort
end
---@return string 返回格式：mm:ss
function LuaTimeSpanMeta.getters.PMGameUtil_Format_MinutesSecondsShort(self, key)
    local PMGameUtil_Format_MinutesSeconds=string.format("%02d", self.Minute) .. ':' .. string.format("%02d", self.Second)
    rawset(self, key, PMGameUtil_Format_MinutesSeconds)
    return PMGameUtil_Format_MinutesSeconds
end
---@return string 返回格式：s秒
function LuaTimeSpanMeta.getters.PMGameUtil_Format_Seconds(self, key)
    SecondsText = SecondsText or ConfigMgr:FromStringTable(StringTablePath.ST_Common, "PMGameUtil_Format_Seconds")
    local InPMGameUtil_Format_Seconds = ObjectUtil:GetTextFromFormat(SecondsText, {Seconds = self.Seconds})
    rawset(self, key, InPMGameUtil_Format_Seconds)
    return InPMGameUtil_Format_Seconds
end
---@return string 返回格式：hh:mm:ss
function LuaTimeSpanMeta.getters.PMGameUtil_Format_HoursMinutesSeconds(self, key)
    HoursMinutesSecondsText =
        HoursMinutesSecondsText or ConfigMgr:FromStringTable(StringTablePath.ST_Common, "PMGameUtil_Format_Hours2")
    local args = {}
    args.Hours = string.format("%01d", self.Hours)
    args.Minutes = string.format("%02d", self.Minute)
    args.Seconds = string.format("%02d", self.Second)
    local InPMGameUtil_Format_HoursMinutesSeconds = ObjectUtil:GetTextFromFormat(HoursMinutesSecondsText, args)
    rawset(self, key, InPMGameUtil_Format_HoursMinutesSeconds)
    return InPMGameUtil_Format_HoursMinutesSeconds
end
---@return string 根据时长范围，仅返回：d天|h小时|m分钟|s秒其中之一
function LuaTimeSpanMeta.getters.PMGameUtil_Format_ExpectUnit(self, key)
    local InPMGameUtil_Format_ExpectUnit
    if self.Day > 0 then
        InPMGameUtil_Format_ExpectUnit = self.PMGameUtil_Format_Days
    elseif self.Hours >= 1 and self.Hours < 24 then
        InPMGameUtil_Format_ExpectUnit = self.PMGameUtil_Format_Hours
    elseif self.Minutes >= 1 and self.Minutes < 60 then
        InPMGameUtil_Format_ExpectUnit = self.PMGameUtil_Format_Minutes
    else
        InPMGameUtil_Format_ExpectUnit = self.PMGameUtil_Format_Seconds
    end
    rawset(self, key, InPMGameUtil_Format_ExpectUnit)
    return InPMGameUtil_Format_ExpectUnit
end
---@return string 根据时长范围，仅返回：d天h小时|h小时m分钟|m分钟s秒|s秒(<1min) 其中之一
function LuaTimeSpanMeta.getters.PMGameUtil_Format_ExpectTwoUnit(self, key)
    local InPMGameUtil_Format_ExpectTwoUnit
    if self.Day > 0 then
        InPMGameUtil_Format_ExpectTwoUnit = self.PMGameUtil_Format_DaysHours
    elseif self.Hours >= 1 and self.Hours < 24 then
        InPMGameUtil_Format_ExpectTwoUnit = self.PMGameUtil_Format_HoursMinutes
    elseif self.Minutes >= 1 and self.Minutes < 60 then
        InPMGameUtil_Format_ExpectTwoUnit = self.PMGameUtil_Format_MinutesSeconds
    else
        InPMGameUtil_Format_ExpectTwoUnit = self.PMGameUtil_Format_Seconds
    end
    rawset(self, key, InPMGameUtil_Format_ExpectTwoUnit)
    return InPMGameUtil_Format_ExpectTwoUnit
end
---以下为当UnixTimestamp为UTC0时区Unix时间戳时可用的属性
---@return string 返回本地时区的日期格式 yyyy年mm月dd日
function LuaTimeSpanMeta.getters.PMGameUtil_Format_Date(self, key)
    local DateText = FunctionUtil:FormatAsDate(self.UnixTimestamp + FunctionUtil:GetTimeZoneOffset())
    rawset(self, key, DateText)
    return DateText
end
---@return string 返回本地时区的日期格式 mm月dd日
function LuaTimeSpanMeta.getters.PMGameUtil_Format_MonthDay(self, key)
    local DateText = FunctionUtil:FormatAsDate(self.UnixTimestamp + FunctionUtil:GetTimeZoneOffset())
    DateText=string.sub(DateText,6)
    rawset(self, key, DateText)
    return DateText
end
---@return string 返回UTC时区的日期格式 yyyy年mm月dd日
function LuaTimeSpanMeta.getters.PMGameUtil_Format_UTCDate(self, key)
    local DateText = FunctionUtil:FormatAsDate(self.UnixTimestamp)
    rawset(self, key, DateText)
    return DateText
end
---@return string 返回本地时区的简短日期格式 yyyy/mm/dd
function LuaTimeSpanMeta.getters.PMGameUtil_Format_DateShort(self, key)
    local DateText = os.date("%Y/%m/%d", self.UnixTimestamp) --local time zone
    rawset(self, key, DateText)
    return DateText
end
---@return string 返回本地时区的简短日期格式(不包含年份) mm/dd
function LuaTimeSpanMeta.getters.PMGameUtil_Format_DateShorter(self, key)
    local DateText = os.date("%m/%d", self.UnixTimestamp) --local time zone
    rawset(self, key, DateText)
    return DateText
end
---@return string 返回UTC时区的简短日期格式 yyyy/mm/dd
function LuaTimeSpanMeta.getters.PMGameUtil_Format_UTCDateShort(self, key)
    local DateText = os.date("!%Y/%m/%d", self.UnixTimestamp) --UTC time zone
    rawset(self, key, DateText)
    return DateText
end
---@return string 返回本地时区的时间格式 HH:MM:SS
function LuaTimeSpanMeta.getters.PMGameUtil_Format_Time(self, key)
    local TimeText = os.date("%X", self.UnixTimestamp) --local time zone
    rawset(self, key, TimeText)
    return TimeText
end
---@return string 返回UTC时区的时间格式 HH:MM:SS
function LuaTimeSpanMeta.getters.PMGameUtil_Format_UTCTime(self, key)
    local TimeText = os.date("!%X", self.UnixTimestamp) --UTC time zone
    rawset(self, key, TimeText)
    return TimeText
end
---@return string 返回本地时区的时间格式（省略秒） HH:MM
function LuaTimeSpanMeta.getters.PMGameUtil_Format_TimeShort(self, key)
    local TimeText = os.date("%H:%M", self.UnixTimestamp) --local time zone
    rawset(self, key, TimeText)
    return TimeText
end
---@return string 返回UTC时区的时间格式（省略秒） HH:MM
function LuaTimeSpanMeta.getters.PMGameUtil_Format_UTCTimeShort(self, key)
    local TimeText = os.date("!%H:%M", self.UnixTimestamp) --UTC time zone
    rawset(self, key, TimeText)
    return TimeText
end

function LuaTimeSpanMeta.__index(self, key)
    local getter = LuaTimeSpanMeta.getters[key]
    return getter and getter(self, key)
end
function LuaTimeSpanMeta.__next(self, key)
    local nextKey = next(LuaTimeSpanMeta.getters, key)
    if nextKey then
        return nextKey, self[nextKey]
    else
        return nil
    end
end
function LuaTimeSpanMeta.__pairs(self)
    return LuaTimeSpanMeta.__next, self
end

---@param UnixTimestamp number UTC时间戳或秒数
---@return LuaTimeSpan
function FunctionUtil:FormatTime(UnixTimestamp)
    return setmetatable({UnixTimestamp = UnixTimestamp}, LuaTimeSpanMeta)
end

--计算字符长度 中文字长度也为一  如果不做处理 中文字符长度为3
function FunctionUtil:getByteCount(str)
    if string.IsNilOrEmpty(str) then
        return 0
    end
    local realByteCount = #str
    local length = 0
    local curBytePos = 1
    while (true) do
        local step = 1 --遍历字节的递增值
        local byteVal = string.byte(str, curBytePos)
        byteVal = byteVal or 1
        if byteVal > 239 then
            step = 4
        elseif byteVal > 223 then
            step = 3
        elseif byteVal > 191 then
            step = 2
        else
            step = 1
        end
        curBytePos = curBytePos + step
        length = length + 1
        if curBytePos > realByteCount then
            break
        end
    end
    return length
end

function FunctionUtil:getSubStringByCount(str, startIndex, endIndex)
    if not str then
        return ""
    end
    local realByteCount = #str
    if endIndex >= realByteCount then
        return str
    end
    local length = 0
    local curBytePos = 1
    local newStr = ""
    while (true) do
        local step = 1 --遍历字节的递增值
        local byteVal = string.byte(str, curBytePos)

        if byteVal > 239 then
            step = 4
        elseif byteVal > 223 then
            step = 3
        elseif byteVal > 191 then
            step = 2
        else
            step = 1
        end
        curBytePos = curBytePos + step
        length = length + 1
        if length == startIndex then
            startIndex = curBytePos - step
        end
        if length == endIndex then
            newStr = string.sub(str, startIndex, curBytePos - 1)
            break
        end
        if curBytePos > realByteCount then
            break
        end
    end
    return newStr
end

-- example 
-- input : 我们123
-- output: {"我","们","1","2","3"}
function FunctionUtil:getSubStringListByCount(str)
    if not str or str == "" then
        return {}
    end
    local ret = {}
    local realByteCount = #str
    local length = 0
    local curBytePos = 1
    local newStr = ""
    while (true) do
        local step = 1 --遍历字节的递增值
        local byteVal = string.byte(str, curBytePos)

        if byteVal > 239 then
            step = 4
        elseif byteVal > 223 then
            step = 3
        elseif byteVal > 191 then
            step = 2
        else
            step = 1
        end
        ret[#ret + 1] =  string.sub(str, curBytePos, curBytePos+step-1)
        curBytePos = curBytePos + step
        length = length + 1

        if curBytePos > realByteCount then
            break
        end
    end
    return ret
end

--将utf8字符转换为unicode编码格式对应的十进制数值
function FunctionUtil:utf8_to_unicode(convertStr)
    if type(convertStr)~="string" then
        return convertStr
    end
    local resultDec= 0
    local i=1
    local num1=string.byte(convertStr,i)
    if num1 ~= nil then
        local tempVar1,tempVar2 = 0, 0
        if num1 >= 0x00 and num1 <= 0x7f then
            tempVar1=num1
            tempVar2=0
        elseif num1 & 0xe0 == 0xc0 then
            local t1 = 0
            local t2 = 0
            t1 = num1 & 0xff >> 3
            i=i+1
            num1= string.byte(convertStr,i)
            t2 =  num1 &  0xff>> 2
            tempVar1= t2 | ((t1 & (0xff >> 6)) << 6)
            tempVar2= t1 >> 2
        elseif num1 & 0xf0== 0xe0 then
            local t1 = 0
            local t2 = 0
            local t3 = 0
            t1 = num1 & (0xff >> 3)
            i=i+1
            num1= string.byte(convertStr,i)
            t2 = num1 & (0xff >> 2)
            i=i+1
            num1= string.byte(convertStr,i)
            t3 = num1 & (0xff >> 2)
            tempVar1=((t2 & (0xff >> 6)) << 6) | t3
            tempVar2=(t1 << 4) | (t2 >> 2)
        end
        resultDec =  tempVar2 * 256 + tempVar1
    end
    return resultDec
end

function FunctionUtil:CheckSpecial(char, listRange)
    listRange = listRange or {
        {"0x4E00","0x9FFF"},
    }
    local strip16 = function (num16)
        local s = num16
        if s[1] == "0" and string.upper(s[2]) == "X" then
            local len = string.len(s)
            s = string.sub(num16, 3, len)
        end
        return tonumber(s, 16)
    end

    local num = self:utf8_to_unicode(char)
    for i, v in listRange do
        if v[1] and v[2] then
            local startNum = strip16(v[1])
            local endNum = strip16(v[2])
            if startNum <= num and num <= endNum then
                return true
            end
        end
    end
    return false
end


--字符串切割
function FunctionUtil:Split(str, reps)
    local resultStrList = {}
    string.gsub(
        str,
        "[^" .. reps .. "]+",
        function(w)
            table.insert(resultStrList, w)
        end
    )
    return resultStrList
end

function FunctionUtil:urlEncode(s)
    s =
        string.gsub(
        s,
        "([^%w%.%- ])",
        function(c)
            return string.format("%%%02X", string.byte(c))
        end
    )
    return string.gsub(s, " ", "+")
end

function FunctionUtil:urlDecode(s)
    s =
        string.gsub(
        s,
        "%%(%x%x)",
        function(h)
            return string.char(tonumber(h, 16))
        end
    )
    return s
end
--是否包含非法字符
function FunctionUtil:have_illegal_char(nick)
    local k = 1
    while true do
        if k > #nick then
            break
        end
        local c = string.byte(nick, k)
        if not c then
            break
        end
        local is_ascii = (c >= 0x00 and c <= 0x7F)
        local is_number = (c >= 0x30 and c <= 0x39) --数字
        local is_letter = (c >= 0x41 and c <= 0x5A) or (c >= 0x61 and c <= 0x7A) --字母
        local is_allow = (c == 0x2D) --允许的符号
        if is_ascii and not is_number and not is_letter and not is_allow then
            return true
        end
        k = k + 1
    end
    return false
end
function FunctionUtil:randomTable(_table, _num)
    local _result = {}
    local _index = 1
    local _num = _num or #_table
    while #_table ~= 0 do
        local ran = math.random(1, #_table)
        if _table[ran] ~= nil then
            _result[_index] = _table[ran]
            table.remove(_table, ran)
            _index = _index + 1
            if _index > _num then
                break
            end
        end
    end
    return _result
end
function FunctionUtil:RemoveByValue(_table, value)
    for k, v in pairs(_table) do
        if v == value then
            table.remove(_table, k)
            return true
        end
    end
    return false
end

function FunctionUtil:GetCountDownTimeText(countDownTime, stopCallback, format)
    if not format then
        format = "PMGameUtil_Format_ExpectTwoUnit"
    end
    local CountDownTimeText = ""
    if countDownTime < 0 then
        if stopCallback then
            stopCallback()
        end
        return CountDownTimeText
    end
    local timeTable = FunctionUtil:FormatTime(countDownTime)
    CountDownTimeText = timeTable[format]

    return CountDownTimeText
end

---国际版本地化相关，生成价格货币文本展示
---@param BaseValue number 货币数量
---@param CurrencyCode string 货币代码，ISO-4217标准，如人民币:CNY 美元:USD 欧元:EUR等
---@return string 本地化友好的货币符号展示，如(123456,'CNY')->'￥1,234.56';
function FunctionUtil:AsCurrencyBase(BaseValue, CurrencyCode)
    return UE4.UKismetTextLibrary.AsCurrencyBase(BaseValue, CurrencyCode)
end

---获取服务器当前时间戳，以UTC+0时区为准
---@return number Unix时间戳
function FunctionUtil:GetServerTime()
    return UE4.UPMLuaBridgeBlueprintLibrary.GetServerTime()
end

---获取当地时区的当前时间戳，根据服务器时间加客户端当前时区偏移计算得到
---@return number Unix时间戳
function FunctionUtil:GetLocalizeTime()
    return UE4.UPMLuaBridgeBlueprintLibrary.GetLocalizeTime()
end

---获得本地时区与UTC时差Unix时间偏移(秒)
---@return number 秒数
function FunctionUtil:GetTimeZoneOffset()
    return UE4.UPMLuaBridgeBlueprintLibrary.GetTimeZoneOffset()
end

---根据Unix时间戳返回FDateTime时间结构
---@return FDateTime UE时间结构
function FunctionUtil:GetDateTimeFromUnix(timestamp)
    return UE4.UPMLuaBridgeBlueprintLibrary.GetDateTimeFromUnix(timestamp)
end

---根据Unix时间戳返回FTimespan时间差（EndTime-StartTime）
---@param startTime number 开始时间戳
---@param endTime number 结束时间戳， 可省略则以startTime为总秒数
---@return FTimespan UE时差结构
function FunctionUtil:GetTimeSpanFromUnix(startTime, endTime)
    if endTime == nil then
        endTime = startTime
        startTime = 0
    end
    return UE4.UPMLuaBridgeBlueprintLibrary.GetTimeSpanFromUnix(startTime, endTime)
end

---根据Unix时间戳返回日期文本
---@param timestamp number|FDateTime 支持传入unix时间戳或UE的FDateTime
---@param dateStyle EDateTimeStyle枚举 输出格式 默认0，1简短，2正常，3长，4全
---@return string
function FunctionUtil:FormatAsDate(timestamp, dateStyle)
    if type(timestamp) == "number" then
        timestamp = FunctionUtil:GetDateTimeFromUnix(timestamp)
    end
    return UE4.UPMLuaBridgeBlueprintLibrary.FormatAsDate(timestamp, dateStyle or 0)
end

---根据Unix时间戳返回日期时间文本
---@param timestamp number|FDateTime 支持传入unix时间戳或UE的FDateTime
---@return string
function FunctionUtil:FormatAsDateTime(timestamp, dateStyle, timeStyle)
    if type(timestamp) == "number" then
        timestamp = FunctionUtil:GetDateTimeFromUnix(timestamp)
    end
    return UE4.UPMLuaBridgeBlueprintLibrary.FormatAsDateTime(timestamp, dateStyle or 0, timeStyle or 0)
end

---根据Unix时间戳返回时间文本
---@param timestamp number|FDateTime 支持传入unix时间戳或UE的FDateTime
---@return string
function FunctionUtil:FormatAsTime(timestamp, timeStyle)
    if type(timestamp) == "number" then
        timestamp = FunctionUtil:GetDateTimeFromUnix(timestamp)
    end
    return UE4.UPMLuaBridgeBlueprintLibrary.FormatAsTime(timestamp, timeStyle or 0)
end

---根据时间差返回时长文本
---@param deltaTime number|FTimespan 支持传入秒数或UE的FTimespan
---@return string
function FunctionUtil:FormatAsTimespan(deltaTime)
    if type(deltaTime) == "number" then
        deltaTime = FunctionUtil:GetTimeSpanFromUnix(deltaTime)
    end
    return UE4.UKismetTextLibrary.AsTimespan_Timespan(deltaTime)
end

---根据操作系统当前语言得到最合适的语言，找不到回退英语en
---@return string
function FunctionUtil:GetDefaultLanguage(osLang)
    local availableCultures = UE4.TArray(UE4.FString)
    availableCultures:Add("zh-Hans")
    availableCultures:Add("en")
    availableCultures:Add("ja")
    osLang = osLang or UE4.UKismetSystemLibrary.GetDefaultLanguage()
    osLang = UE4.UKismetInternationalizationLibrary.GetSuitableCulture(availableCultures, osLang, "en")
    return osLang
end

---设置更改当前语言
---@param lang string |'"zh-Hans"'|'"en"'|'"ja"'
function FunctionUtil:SetCurrentLanguage(lang)
    UE4.UKismetInternationalizationLibrary.SetCurrentCulture(lang, true)
end

---返回当前使用的语言
---@return string
function FunctionUtil:GetCurrentLanguage()
    local lang = UE4.UKismetInternationalizationLibrary.GetCurrentCulture()
    return lang
end

---去除字符串(日期)前面的0
---@return string
function FunctionUtil:RemoveLeadingZeros(str)
    return tostring(tonumber(str))
end

function FunctionUtil:GetRemainingTimeStrFromTimeStamp(timeStamp)
    local str = ""
    if timeStamp <= 0 then
        return str
    end

    local day = math.floor(timeStamp / (24 * 60 * 60))
    local hour = math.floor(timeStamp % (24 * 60 * 60) / (60 * 60))
    local minute = math.floor(timeStamp % (24 * 60 * 60) % (60 * 60) / 60)
    local seconds = math.floor(timeStamp % (24 * 60 * 60) % (60 * 60) % 60)
    local hourStr = tostring(day * 24 + hour)
    local minuteStr = tostring(minute)
    local secondsStr = tostring(seconds)
    str = hourStr .. tostring(":") .. minuteStr .. tostring(":") .. secondsStr
    return str
end

--由时间戳转换为分：秒,默认显示00:00
-- function FunctionUtil:GetMinuteTimeStrFromTimeStamp(timeStamp)
--     local timeStr = "00:00"
--     if timeStamp > 0 then
--         local minute = math.floor(timeStamp % (24 * 60 * 60) % (60 * 60) / 60)
--         local seconds = math.floor(timeStamp % (24 * 60 * 60) % (60 * 60) % 60)
--         local minuteStr = tostring(minute)
--         local secondsStr = tostring(seconds)
--         if minute < 10 then
--             minuteStr = "0".. minuteStr
--         end
--         if seconds < 10 then
--             secondsStr = "0".. secondsStr
--         end
--         timeStr = os.date("%H:%M", timeStamp)
--         --minuteStr .. tostring(":") .. secondsStr
--     end
--     return timeStr
-- end

function FunctionUtil:IfNeedFrameItem(itemID)
    local GameFacade = _G.GameFacade
    local ProxyNames = _G.ProxyNames
    local ItemType = GameFacade:RetrieveProxy(ProxyNames.ItemsProxy):GetItemIdIntervalType(itemID)
    if
        ItemType == UE4.EItemIdIntervalType.BagItem_WeaponCard or
            ItemType == UE4.EItemIdIntervalType.BagItem_RoleSkinCard
     then
        return true
    else
        return false
    end
end

function FunctionUtil:IfNeedDisplayItem(itemID)
    local GameFacade = _G.GameFacade
    local ProxyNames = _G.ProxyNames
    local ItemType = GameFacade:RetrieveProxy(ProxyNames.ItemsProxy):GetItemIdIntervalType(itemID)
    if ItemType == UE4.EItemIdIntervalType.BagItem_Undisplay then
        return true
    else
        return false
    end
end

---@param type ETaskGoToType
---@param closePageList table   要关闭的页面列表
function FunctionUtil:ProcessGoToLogic(gotoType, closePageList)
    local jump = false
    if gotoType == GlobalEnumDefine.ETaskGoToType.Play then
        local NavBarBodyTable = {
            pageType = UE4.EPMFunctionTypes.Play
        }
        GameFacade:SendNotification(NotificationDefines.NavigationBar.SwitchDisplayPage, NavBarBodyTable)
        jump = true
    elseif gotoType == GlobalEnumDefine.ETaskGoToType.BPTask then
        local NavBarBodyTable = {
            pageType = UE4.EPMFunctionTypes.BattlePass,
            secondIndex = 3
        }
        GameFacade:SendNotification(NotificationDefines.NavigationBar.SwitchDisplayPage, NavBarBodyTable)
        jump = true
    elseif gotoType == GlobalEnumDefine.ETaskGoToType.ApartmentMainGift then
        if GameFacade:RetrieveProxy(ProxyNames.BasicFunctionProxy):IsApartmentOpen() then
            local EnumClickButton = {Promise = 0, Information = 1, Memory = 2, Gift = 3}
            local NavBarBodyTable = {
                pageType = UE4.EPMFunctionTypes.Apartment,
                exData = {EnumClickButton = EnumClickButton.Gift}
            }
            GameFacade:SendNotification(NotificationDefines.NavigationBar.SwitchDisplayPage, NavBarBodyTable)
            jump = true
        else
            ShowCommonTip()
        end
    end

    if jump and closePageList and type(closePageList) == 'table' then
        for _, pageName in ipairs(closePageList) do
            if UIPageNameDefine[pageName] then
                ViewMgr:ClosePage(LuaGetWorld(), UIPageNameDefine[pageName])
            end
        end
    end
end

--检测中文、数字、字母
function FunctionUtil:check_nick_name(nick)
    local ss = {}
    local k  = 1
    while true do
        if k > #nick then
            break
        end
        local c = string.byte(nick, k)
        if not c then
            break
        end
        if (c >= 48 and c <= 57) or (c >= 65 and c <= 90) or (c >= 97 and c <= 122) then
            ss[#ss + 1] = string.char(c)
        elseif c >= 228 and c <= 233 then
            local c1 = string.byte(nick, k + 1)
            local c2 = string.byte(nick, k + 2)
            if c1 and c2 then
                local a1, a2, a3, a4 = 128, 191, 128, 191
                if c == 228 then
                    a1 = 184
                elseif c == 233 then
                    a2, a4 = 190, c1 ~= 190 and 191 or 165
                end
                if c1 >= a1 and c1 <= a2 and c2 >= a3 and c2 <= a4 then
                    k           = k + 2
                    ss[#ss + 1] = string.char(c, c1, c2)
                end
            end
        end
        k = k + 1
    end
    local len = 0
    if #ss > 0 then
        for _, v in pairs(ss) do
            len = len + #v
        end
    end
    return #nick == len
end

function FunctionUtil:CheckCanPlayAvg()
    local canPlay = true
    local roomDataProxy = GameFacade:RetrieveProxy(ProxyNames.RoomDataProxy)
    if roomDataProxy:GetIsInMatch() then
        canPlay = false
        local tipsMsg = UE4.UKismetTextLibrary.TextFromStringTable(StringTablePath.ST_Apartment, "ForbiddenAvgForMatching")
        GameFacade:SendNotification(NotificationDefines.ShowCommonTipCmd, tipsMsg)
    end
    return canPlay
end

return FunctionUtil
