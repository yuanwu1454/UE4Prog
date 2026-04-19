-- ##requires
local SuperClass = PureMVC.GamePage
local SettingPage = class("SettingPage", SuperClass)

local SettingPageMediator = require("Business/Setting/Mediator/SettingPageMediator")

function SettingPage:ListNeededMediators()
    return {
        SettingPageMediator
    }
end


function SettingPage:Construct()
    print("SettingPage Construct")
    SuperClass.Construct(self)
    self:CreateBindEvent("Button_Click1", "OnClicked", "OnClick1")
    self:CreateBindEvent("Button_Click2", "OnClicked", "OnClick2")
    self:CreateBindEvent("Button_Click3", "OnClicked", "OnClick3")
    self:CreateBindEvent("Button_Click4", "OnClicked", "OnClick4")
    self.WBP_HotKey:SetClickFunc(function()
        self:OnCloseClick()
    end)

    local player = LuaGetPlayerController()
    if player then
        player:EnterUIOnlyInputMode()
    end
end

function SettingPage:OnApplyViewportResized()
    print("applyviewportresized")
end

function SettingPage:Destruct()
    SuperClass.Destruct(self)
    print("SettingPage Destruct")
    local player = LuaGetPlayerController()
    if player then
        player:EnterGameOnlyInputMode()
    end
end

function SettingPage:OnOpen(luaData, originOpenData)
end


function SettingPage:OnClick1()
    LogInfo(" SettingPage ", "OnClick1");
    UE4.UMyLuaBridgeBlueprintLibrary.Test()
end


function SettingPage:OnClick2()
    LogInfo(" SettingPage ", "OnClick2");
end

function SettingPage:OnClick3()
    LogInfo(" SettingPage ", "OnClick3");

    UE4.UGameplayStatics.OpenLevel(self, "/Game/Cinema/King", false)
    -- local USequenceMapManager = UE4.USequenceMapManager.Get(self)
    -- USequenceMapManager:TeleportMap(97001)
    -- print(ConfigMgr:FromStringTable(StringTablePath.ST_Common, "OK"))

    -- local bpclass = ObjectUtil.LoadUIBPClass("SettingPage")

    -- local M = require("LuaProto/test")
    -- M.Test()
end

function SettingPage:OnClick4()
    LogInfo(" SettingPage ", "OnClick4");
    -- print(ND.ManaNotify)
    local class =  UE4.UClass.Load("/Game/Actor/BP_MeshActor.BP_MeshActor_C")
    local world = LuaGetWorld()
    local Arr = UE4.UGameplayStatics.GetAllActorsOfClass(world, class)
    print(Arr:Length())
    if Arr:Length() > 0 then
        local actor = Arr:Get(1)
        if actor then
            local mesh = actor.SkeletalMeshComp
            if mesh then
                mesh:Stop()
            end
        end

    end

end

function SettingPage:OnCloseClick()
    LogInfo(" SettingPage ", "OnCloseClick");

    UE4.LuaBridge.LuaClosePage(self, true)
end


function SettingPage:LuaHandleKeyEvent(Key, InputEvent)
    if self.WBP_HotKey then
        local bRet = self.WBP_HotKey:MonitorKeyDown(Key, InputEvent)
        if bRet then
            return true
        end
    end
    return false
end

return SettingPage
