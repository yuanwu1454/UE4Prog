local PureMVC = _G.PureMVC
PureMVC.GamePanel = require('Adapter/GamePanel')
PureMVC.GamePage = require('Adapter/GamePage')
PureMVC.ModuleInit = require('Adapter/ModuleInit')

--获取当前UWorld
local function LuaGetWorld()
    local g_LuaBridgeSubsystem = _G.g_LuaBridgeSubsystem
    return g_LuaBridgeSubsystem and g_LuaBridgeSubsystem:GetLuaDefaultWorld() or nil
end

local function LuaGetPlayerController(context)
    local world = context or LuaGetWorld()
    return UE4.UGameplayStatics.GetPlayerController(world, 0)
end


local function LuaGetPlayerController(context)
    local world = context or LuaGetWorld()
    return UE4.UGameplayStatics.GetPlayerController(world, 0)
end

local function LuaGetGameMode(context)
    local world = context or LuaGetWorld()
    return UE4.UGameplayStatics.GetGameMode(world)
end

local function LuaGetGameState(context)
    local world = context or LuaGetWorld()
    return UE4.UGameplayStatics.GetGameState(world)
end

_G.LuaGetWorld = LuaGetWorld
_G.LuaGetPlayerController = LuaGetPlayerController
_G.LuaGetGameMode = LuaGetGameMode
_G.LuaGetGameState = LuaGetGameState
