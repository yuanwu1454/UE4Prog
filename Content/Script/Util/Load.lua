local ObjectUtil = {}

local globalIndex = 0
function ObjectUtil.CreateLuaUObject(OuterUObject, Name)
    local UObjectClass = UE4.UClass.Load("/Script/PMGame.PMLuaBaseObject")
    local LuaBindingPath = "base/luabindings/BaseUObject"
    globalIndex = globalIndex + 1
    return NewObject(UObjectClass, OuterUObject, Name or 'BaseUObject' .. globalIndex, LuaBindingPath)
end


function ObjectUtil.CreateLuaUObjectExt(OuterUObject, ClassType, LuaModule, Name)
    globalIndex = globalIndex + 1
    return NewObject(ClassType, OuterUObject, Name or 'BaseUObject' .. globalIndex, LuaModule)
end

--根据UserWidget名获取并加载对应的蓝图Widget，用于Open指定的Page
function ObjectUtil.LoadUIBPClass(WidgetName)
    local DataRows = ConfigMgr:GetUITableRows()
    local WidgetDataRow = DataRows.GetRow(DataRows, WidgetName)
    local PagePath = UE4.UKismetSystemLibrary.BreakSoftClassPath(WidgetDataRow.ViewClass)
    LogDebug("ObjectUtil", "Page Path: " .. PagePath)
    if PagePath ~= "" then
        local PageClass = UE4.UClass.Load(PagePath)
        if PageClass:IsValid() then
            return PageClass
        end
    end
    LogDebug("ObjectUtil", "Page path or class is not exist")
    return nil
end

--根据FSoftObjectReference加载并返回指定类
function ObjectUtil.LoadClass(SoftObjectRef)
    local ClassPath = UE4.UKismetSystemLibrary.Conv_SoftClassReferenceToString(SoftObjectRef)
    LogDebug("ObjectUtil", "Class path: " .. ClassPath)
    if ClassPath ~= "" then
        local ClassToLoad = UE4.UClass.Load(ClassPath)
        if ClassToLoad and ClassToLoad:IsValid() then
            return ClassToLoad
        end
    end
    LogDebug("ObjectUtil", "Class path or class is not exist")
    return nil
end

--设置UI的TextBlock字体颜色，默认白色
function ObjectUtil.SetTextColor(textBlock, r, g, b, a)
	if textBlock == nil then
		return
	end
	local red = r or 1
	local green = g or 1
	local blue = b or 1
	local alpha = a or 1
    local color = UE4.FSlateColor()
    color.SpecifiedColor = UE4.UKismetMathLibrary.MakeColor(red, green, blue, alpha)
    textBlock:SetColorAndOpacity(color)
end

---转换格式字符串，调用引擎TextFormat支持参数修饰符，参数值支持字符串、整形或浮点数值。
---@param formatString string 文本格式
---@param stringMap table<string, any> 命名参数表
---@param useNumberArgs boolean 若为true则参数数值会以本地化友好展示比如12345->12,345，默认则直接tostring 
---@return string
function ObjectUtil.GetTextFromFormat(formatString, stringMap, useNumberArgs)
	if not formatString or not stringMap then
		return formatString
	end
	local arrayArgs = UE4.TArray(UE4.FFormatArgumentData)
	for key, value in pairs(stringMap) do
		local arg = UE4.FFormatArgumentData()
		arg.ArgumentName = tostring(key)
		if type(value) == "number" and useNumberArgs then
			if math.isinteger(value) then
				arg.ArgumentValueInt = value
				arg.ArgumentValueType = 0
			else
				arg.ArgumentValueFloat = value
				arg.ArgumentValueType = 2
			end
		else
			arg.ArgumentValue = tostring(value)
			arg.ArgumentValueType = 4
		end
		arrayArgs:Add(arg)
	end
	return UE4.UKismetTextLibrary.Format(formatString, arrayArgs)
end

function ObjectUtil.GetTextFromFormatByST(STPathID, STPathKey, stringMap, useNumberArgs)
    local formatString = UE4.UKismetTextLibrary.TextFromStringTable(STPathID, STPathKey)
    return self:GetTextFromFormat(formatString, stringMap, useNumberArgs)
end

-- return ObjectUtil
_G.Load = Load