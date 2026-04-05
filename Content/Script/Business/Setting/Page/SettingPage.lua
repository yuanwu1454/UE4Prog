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
    SuperClass.Construct(self)
    self:CreateBindEvent("Button_Click1", "OnClicked", "OnClick1")
    self:CreateBindEvent("Button_Click2", "OnClicked", "OnClick2")
    self:CreateBindEvent("Button_Click3", "OnClicked", "OnClick3")
    self:CreateBindEvent("Button_Close", "OnClicked", "OnCloseClick")

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


function SettingPage:OnClick1()
    LogInfo(" SettingPage ", "OnClick1");
    UE4.UMyLuaBridgeBlueprintLibrary.Test()
end


function SettingPage:OnClick2()
    LogInfo(" SettingPage ", "OnClick2");
end

function SettingPage:OnClick3()
    LogInfo(" SettingPage ", "OnClick3");
end

function SettingPage:OnCloseClick()
    LogInfo(" SettingPage ", "OnCloseClick");

    UE4.LuaBridge.LuaClosePage(self, true)
end


function SettingPage:LuaHandleKeyEvent(Key, InputEvent)
    if Key.KeyName == "Escape" and InputEvent == UE4.EInputEvent.IE_Released then
        self:OnCloseClick()
        return true;
    end
    return false;
end

return SettingPage
