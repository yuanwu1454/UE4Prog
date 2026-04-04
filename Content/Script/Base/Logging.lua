local _G = _G
_G.print = _G.UEPrint

local logLevel = {}
logLevel.fatal = 1
logLevel.Error = 2
logLevel.Warning = 3
logLevel.Display = 4
logLevel.Log = 5
logLevel.Verbose = 6
logLevel.VeryVerbose = 7
logLevel.All = 8

_G.LogLevel = logLevel.All

local WithoutLogging = function (log_level)
    if (_G.NO_LOGGING) then
        return true
    end
    if (log_level > _G.LogLevel) then
        return true
    end
end

-- format log
local GetLogContent = function(tag, format, ...)
    if (tag and not format) then
        return tag
    else
        return string.format('[%s] ' .. format, tag, ...)
    end
end

_G.DisableLog = function ()
    _G.NO_LOGGING = true
   LogInfo('lualog', 'disable lua log')
end

_G.SetLogLevel = function (level)
   LogInfo('lualog', 'Set Log %s to %s', tostring(_G.LogLevel), tostring(level))
   _G.LogLevel = level
end

_G.IsLogDebugEnable = function()
    return not WithoutLogging(logLevel.Verbose)
end

_G.LogDebug = function(tag, format, ...)
    if not _G.IsLogDebugEnable() then return end
    local log = GetLogContent(tag, format, ...)
    UE4.UMyLuaBridgeBlueprintLibrary.Lua_UELOG(logLevel.Display, log)
end

_G.IsLogInfoEnable = function()
    return not WithoutLogging(logLevel.Log)
end

_G.LogInfo = function(tag, format, ...)
    if not _G.IsLogInfoEnable() then return end
    local log = GetLogContent(tag, format, ...)
    UE4.UMyLuaBridgeBlueprintLibrary.Lua_UELOG(logLevel.Log, log)
end

_G.IsLogWarnEnable = function()
    return not WithoutLogging(logLevel.Warning)
end

_G.LogWarn = function(tag, format, ...)
    if not _G.IsLogWarnEnable() then return end
    local log = GetLogContent(tag, format, ...)
    UE4.UMyLuaBridgeBlueprintLibrary.Lua_UELOG(logLevel.Warning, log)
end

_G.IsLogErrorEnable = function()
    return not WithoutLogging(logLevel.Error)
end

_G.LogError = function(tag, format, ...)
    if not _G.IsLogErrorEnable() then return end
    local log = GetLogContent(tag, format, ...)
    if debug then
        log = log .. debug.traceback('\nLua Error:')
    end
    UE4.UMyLuaBridgeBlueprintLibrary.Lua_UELOG(logLevel.Error, log)
end

_G.LogErrorP = function(tag, format, ...)
    if not _G.IsLogErrorEnable() then return end
    local log = GetLogContent(tag, format, ...)
    UE4.UMyLuaBridgeBlueprintLibrary.Lua_UELOG(logLevel.All, log)
end
