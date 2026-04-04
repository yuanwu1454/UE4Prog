--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type ClassName
local M = UnLua.Class()

function M:Initialize(Initializer)
    LogInfo(" LuaAnimInstance ", tostring("Initialize"));
    
end

function M:BlueprintInitializeAnimation()
    LogInfo(" LuaAnimInstance ", tostring("BlueprintInitializeAnimation"));
end

function M:BlueprintBeginPlay()
    LogInfo(" LuaAnimInstance ", tostring("BlueprintBeginPlay"));
end

local bprint = false
function M:BlueprintUpdateAnimation(DeltaTimeX)
    local pawn = self:TryGetPawnOwner()
    if pawn then
        local rotation = pawn:K2_GetActorRotation()
        local velocity = pawn:GetVelocity()
        local velocityLength = UE4.UKismetMathLibrary.VSize(velocity)
        self.Speed = velocityLength
        self.Direction = self:CalculateDirection(velocity, rotation)
        if not bprint then
            bprint = true
            -- print("name >>>", tostring(pawn.Object))
            -- print("name >>>", tostring(pawn:GetName()))
            -- print("name >>>", tostring(pawn:GetOuter()))
            -- print("name >>>", tostring(pawn:GetWorld()))
            -- print("name >>>", tostring(pawn:GetClass()))
            local charactClassPath = FuncUtil:GetClassByObjPath(CharacterMap.Ue4ASP_Character)
            local class = UE4.UClass.Load(charactClassPath)
            -- print("name >>>", pawn:IsA(class))
        end

        if pawn.IsJumpButtonDown and self.CanJump then
            local bCanJump = pawn:IsJumpButtonDown()
            if self:CanJump(bCanJump) then
                self["Enable Jump"] = true
            end
        end
        if pawn.IsCrouchDown then
            self.Crouching = pawn:IsCrouchDown()
        end

    end
end

function M:BlueprintPostEvaluateAnimation()
    -- LogInfo(" LuaAnimInstance ", tostring("BlueprintPostEvaluateAnimation"));
end

return M