-- ##requires
local SuperClass = PureMVC.GamePanel
local HotKey = class("HotKey", SuperClass)

function HotKey:Construct()
    print("HotKey Construct")
    SuperClass.Construct(self)
    self:CreateBindEvent("Button_Click", "OnClicked", "OnClickButton")
end

function HotKey:Destruct()
    SuperClass.Destruct(self)
end

function HotKey:LuaHandleKeyEvent(Key, InputEvent)
    if Key.KeyName == "Escape" and InputEvent == UE4.EInputEvent.IE_Released then
        self:OnCloseClick()
        return true;
    end
    return false;
end

function HotKey:MonitorKeyDown(Key, InputEvent);
    if InputEvent == UE4.EInputEvent.IE_Released then
        if Key == self.ReceiveKey then
            self:OnClickButton()
            return true
        end
    end
end

function HotKey:OnClickButton()
    print("OnClickButton")
    if self.clickFunc then
        self.clickFunc()
    end
end

function HotKey:SetClickFunc(clickFunc)
    self.clickFunc = clickFunc
end

return HotKey
