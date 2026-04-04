---@class StringTablePath
local StringTablePath = class('StringTablePath')
local StringPrePath = function(Name)
    return string.format("/Game/Game/StringTable/%s.%s", Name, Name)
end
local PackStringPath = function(Name)
    StringTablePath[Name] = StringPrePath(Name)
end

local StringList = {
    "ST_Common",
}

for i, v in ipairs(StringList) do
    PackStringPath(v)
end

local  GetTableText = function(tableId,key)
    return UE4.UKismetTextLibrary.TextFromStringTable(tableId,key)
end


_G.StringTable = StringTablePath
_G.GetTableText = GetTableText