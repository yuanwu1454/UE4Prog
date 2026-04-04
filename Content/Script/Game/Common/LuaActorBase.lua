
local SuperClass = require("Game.Common.LuaObjectBase")
local LuaActorBase = class('LuaActor', SuperClass)

function LuaActorBase:ListNeededMediators()
    -- self:PackNeedesMeidators(SuperClass.ListNeededMediators(
    -- ))
    return {}
end

function LuaActorBase:Initialize(Initializer)
    -- LogInfo(" LuaActorBase ", "Initialize");

    SuperClass.InitMediator(self)
end

function LuaActorBase:UserConstructionScript()
    -- LogInfo(" LuaActorBase ", "UserConstructionScript");
end


function LuaActorBase:ReceiveBeginPlay()
    -- LogInfo(" LuaActorBase ", "BeginPlay");

end

function LuaActorBase:ReceiveEndPlay()
    -- LogInfo(" LuaActorBase ", "EndPlay");
    SuperClass.UnRegisterMediator(self)
    SuperClass.RemoveAllTimer(self)
end

function LuaActorBase:ReceiveTick(DeltaSeconds)
end


function LuaActorBase:SetLocation(x,y,z)
    local vec3 = FuncUtil:CombinVec3(x, y, z)
    local bSweep  = false
    local bTeleport  = false
    -- bSweep	bool	是否启用碰撞扫掠（防止穿墙）
    self:K2_SetActorLocation(vec3, bSweep, UE4.FHitResult(), bTeleport)
end


function LuaActorBase:SetRotation(x,y,z)
    local vec3 = FuncUtil:CombinVec3(x, y, z)
    local bTeleport  = false
    -- bSweep	bool	是否启用碰撞扫掠（防止穿墙）
    self:K2_SetActorRotation(vec3, bTeleport)
end


function LuaActorBase:SetLocationOffset(x,y,z)
    local vec3 = FuncUtil:CombinVec3(x, y, z)
    local bTeleport  = false
    -- bSweep	bool	是否启用碰撞扫掠（防止穿墙）
    self:K2_AddActorLocalOffset(vec3, bSweep, UE4.FHitResult(), bTeleport)
end

function LuaActorBase:SetRotationOffset(x,y,z)
    local vec3 = FuncUtil:CombinVec3(x, y, z)
    local bTeleport  = false
    -- bSweep	bool	是否启用碰撞扫掠（防止穿墙）
    self:K2_AddActorLocalRotation(vec3, bSweep, UE4.FHitResult(), bTeleport)
end

function LuaActorBase:GetRotation()
    return self:K2_GetActorRotation()
end


function LuaActorBase:SetVisible(bVis)
    return self:SetActorHiddenInGame(not bVis)
end


function LuaActorBase:Destory()
    return self:K2_DestroyActor()
end


function LuaActorBase:ReceiveActorBeginCursorOver()

end

function LuaActorBase:ReceiveActorEndCursorOver()

end


function LuaActorBase:ReceiveActorBeginOverlap(otherActor)

end

function LuaActorBase:ReceiveActorEndOverlap(otherActor)

end

function LuaActorBase:ReceiveDestroyed()

end

function LuaActorBase:SetLuaName(name)
    self:SetActorLabel(name)
    self.luaName = name
end

function LuaActorBase:GetLuaName()
    if self.luaName == nil then
        local name = self:GetActorLabel()
        if name and string.len(name) > 0 then
            return name
        else
            return self:GetName()
        end
    end
    return self.luaName
end


return LuaActorBase