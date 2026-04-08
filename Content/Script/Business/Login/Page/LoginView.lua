-- ##requires
local LoginView = class("LoginView", PureMVC.GamePage)

local LoginViewMediator = require("Business/Login/Mediator/LoginViewMediator")

function LoginView:ListNeededMediators()
    return {
        LoginViewMediator
    }
end


function LoginView:Construct()
    self.super.Construct(self)
    self:CreateBindEvent("Button_Gate", "OnClicked", "OnGateBtn")
    self:CreateBindEvent("ButtonOpen", "OnClicked", "OnOpenBtn")
    self:CreateBindEvent("ButtonToggle", "OnClicked", "OnToggleBtn")
    local gameInstance = UE4.UGameplayStatics.GetGameInstance(LuaGetWorld())
    self.Handler = DelegateMgr:AddDelegate(gameInstance.OnApplyViewportResized, self, 'OnApplyViewportResized')
    self.TextBlock_Show:SetText(GetTableText(StringTablePath.ST_Common, "abc"))
    self:RunNextFrame(function() 
        UE4.UWidgetBlueprintLibrary.SetInputMode_GameOnly(LuaGetPlayerController())
        MyViewManagerInstance:ClosePage("LoginPage")
    end, "close")
end

function LoginView:OnApplyViewportResized()
    print("applyviewportresized")
end

function LoginView:Destruct()
    self.super.Destruct(self)
    print("Login Destruct")

        

    if self.Handler then
        local gameInstance = UE4.UGameplayStatics.GetGameInstance(LuaGetWorld())
        DelegateMgr:RemoveDelegate(gameInstance.OnApplyViewportResized, self.Handler)
        self.Handler = nil
    end
end


function LoginView:OnGateBtn()
    LogInfo(" LoginView ", "OnGateBtn");
    GameFacade:SendNotification(CD.LoginCommand)
end


function LoginView:OnOpenBtn()
    LogInfo(" LoginView ", "OnOpenBtn");
    UE4.UGameplayStatics.OpenLevel(self, '/Game/Game/Level/DefaultPanel.DefaultPanel')
end

function LoginView:OnToggleBtn()
    MyViewManagerInstance:ClosePage("LoginPage")
end

return LoginView
