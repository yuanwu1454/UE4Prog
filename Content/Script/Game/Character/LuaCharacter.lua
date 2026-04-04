local SuperClass = require("Game/Common/LuaActorBase")
local LuaCharacter = class('LuaCharacter', SuperClass)

local LuaCharacterMediator = require("Game/Character/LuaCharacterMediator")

local CrouchTag = "Crouch Button Down"
local JumpTag = "Jump Button Down"
function LuaCharacter:ListNeededMediators()
    return {
        LuaCharacterMediator
    }
end

function LuaCharacter:Initialize(Initializer)
    SuperClass.Initialize(self, Initializer)
    LogInfo(" LuaCharacter ", "Initialize");
    
end

function LuaCharacter:UserConstructionScript()
    SuperClass.UserConstructionScript(self)
    LogInfo(" LuaCharacter ", "UserConstructionScript");
end


function LuaCharacter:ReceiveBeginPlay()
    SuperClass.ReceiveBeginPlay(self)
    LogInfo(" LuaCharacter ", "BeginPlay");
    self:InitAnimInstance()
end

function LuaCharacter:ReceiveEndPlay()
    SuperClass.ReceiveEndPlay(self)
    LogInfo(" LuaCharacter ", "EndPlay");
end

function LuaCharacter:KeyBoardFunc(key)
    LogInfo(" LuaCharacter ", "key" .. tostring(key) .. tostring(key.keyName));

    if key.keyName == "H" then
        MyViewManagerInstance:ShowPage(PageNames.PlayerPage)
    end
end

function LuaCharacter:ABC()
    -- self:InitAnimInstance()
    -- self.LuaAnimInstance:Montage_Play()
    LogInfo(" LuaCharacter ", tostring(self.LuaAnimInstance));
    LogInfo(" LuaCharacter ", tostring(self.LuaAnimInstance.AnimNotify_Jump));
    -- self.LuaAnimInstance:AnimNotify_Jump()
    -- self.LuaAnimInstance:AnimNotify_Jump()
    self.LuaAnimInstance.Crouching = true
    print(self.LuaAnimInstance.XX)
    print(self.LuaAnimInstance:XP())
    print(self["Crouch Button Down"])
    self["Crouch Button Down"] = true
end


function LuaCharacter:Crouch()
    self[CrouchTag] = not self[CrouchTag]
end

function LuaCharacter:InitAnimInstance()
    if self.LuaAnimInstance == nil then
        self.LuaAnimInstance = self.Mesh:GetAnimInstance()
    end
end

function LuaCharacter:IsJumpButtonDown()
    return self[JumpTag]
end

function LuaCharacter:IsCrouchDown()
    return self[CrouchTag]
end


return LuaCharacter