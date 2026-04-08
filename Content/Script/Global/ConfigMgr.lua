--[[
    所有配置数据都从此处取，如果需要新的方法也需要添加到Wrap中
    GetXXXTableRows返回的是一个DataTable的userdata
    换取行数据通过dt.

    local cc = ConfigMgr:GetAchievementTableRows()
]]
local ConfigMgr = {}

local cachedData = {}
local richTextStyleTable = {}

local function WrapDataTable(dt)
    local wrap = {}
    wrap.originDT = dt
    wrap.dTLenght = -1
    wrap.allRowsName = nil
    -- 获取行数据
    wrap.GetRow = function(self, rowName)
        return UE4.UDataTableFunctionLibrary.GetRowDataStructure(self.originDT, tostring(rowName))
    end

    -- 总行数
    wrap.RowCount = function(self)
        if (self.dTLenght == -1) then
            self:GetRowNames()
            if (self.allRowsName) then
                self.dTLenght = self.allRowsName:Length()
            end
        end
        return self.dTLenght
    end

    wrap.ToLuaTable = function(self)
        local tmpTable = {}
        for i = 1, self:RowCount() do
            local tmpRowName = self.allRowsName:Get(i)
            tmpTable[tmpRowName] = self:GetRow(tmpRowName)
        end
        return tmpTable
    end

    -- 是否指行行存在
    wrap.DoesRowExist = function(self, rowName)
        return UE4.UDataTableFunctionLibrary.DoesDataTableRowExist(self.originDT, rowName)
    end

    -- 获取所有行数据的Name
    wrap.GetRowNames = function(self)
        if (not self.allRowsName) then
            self.allRowsName = UE4.UDataTableFunctionLibrary.GetDataTableRowNames(self.originDT)
        end
        return self.allRowsName
    end
    return wrap
end

local function GetTableRowCached(tableRowStructName)
    if (cachedData[tableRowStructName]) then
        return cachedData[tableRowStructName]
    end
    local loadFunc = UE4.UMyLuaBridgeBlueprintLibrary.GetConfigDataTable
    local data = loadFunc(tableRowStructName)
    if (not data) then
        -- LogError('Config', 'No TableRowLoad for %s', tableRowStructName)
        return
    end
    local wrapData = WrapDataTable(data)
    cachedData[tableRowStructName] = wrapData
    return wrapData
end

local function ClearTableRowCached(tableRowStructName)
    cachedData[tableRowStructName] = nil
end

---获取字符串
---@param tableId StringTablePath @表名字
---@param key string @键
---@return string
function ConfigMgr:FromStringTable(tableId, key)
    return UE4.UKismetTextLibrary.TextFromStringTable(tableId,key)
end

--获取文本样式
---@param styleName string @文本样式名称，在DT_RichText表中获取
---@return FTextBlockStyle
function ConfigMgr:GetRichTextStyle(styleName)
    if richTextStyleTable[styleName] then
        return richTextStyleTable[styleName].TextStyle
    end
    LogError("ConfigMgr:GetRichTextStyle","DT_RichTextName Cant Find This Name : " .. styleName)
    return nil
end

--获取UI类型配置数据
function ConfigMgr:GetUITableRows()
	return GetTableRowCached('MyUITableRow')
end

_G.ConfigMgr = ConfigMgr