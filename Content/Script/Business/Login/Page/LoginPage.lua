-- ##requires
local LoginPage = class("LoginPage", PureMVC.GamePage)

local LoginPageMediator = require("Business/Login/Mediator/LoginPageMediator")

function LoginPage:ListNeededMediators()
    return {
        LoginPageMediator
    }
end


function LoginPage:Construct()
    self.super.Construct(self)
    self:CreateBindEvent("Button_Gate", "OnClicked", "OnGateBtn")
    self:CreateBindEvent("ButtonOpen", "OnClicked", "OnOpenBtn")
    self:CreateBindEvent("ButtonToggle", "OnClicked", "OnToggleBtn")
    self:CreateBindEvent("Button_Login", "OnClicked", "OnLoginBtn")

    local player = LuaGetPlayerController()
    if player then
        player:EnterUIOnlyInputMode()
    end
end

function LoginPage:OnApplyViewportResized()
    print("applyviewportresized")
end

function LoginPage:Destruct()
    self.super.Destruct(self)
    print("Login Destruct")

        

    if self.Handler then
        local gameInstance = UE4.UGameplayStatics.GetGameInstance(LuaGetWorld())
        DelegateMgr:RemoveDelegate(gameInstance.OnApplyViewportResized, self.Handler)
        self.Handler = nil
    end
end


function LoginPage:OnGateBtn()
    LogInfo(" LoginPage ", "OnGateBtn");
    GameFacade:SendNotification(CD.LoginCommand)
end


function LoginPage:OnOpenBtn()
    LogInfo(" LoginPage ", "OnOpenBtn");
    UE4.UGameplayStatics.OpenLevel(self, '/Game/Game/Level/DefaultPanel.DefaultPanel')
end

function LoginPage:OnToggleBtn()
    MyViewManagerInstance:ClosePage("LoginPage")
end

function LoginPage:OnLoginBtn()
    print(UE4.UGameplayStatics)
    local instance = UE4.UGameplayStatics.GetGameInstance(self)
    instance:GotoLobbyScene()
end

return LoginPage

