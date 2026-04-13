-- ##requires
local SuperClass = PureMVC.GamePage
local APage = class("APage", SuperClass)

function APage:ListNeededMediators()
    return {
    }
end


function APage:Construct()
    print("APage Construct")
    SuperClass.Construct(self)
    self:CreateBindEvent("Button_Click1", "OnClicked", "OnClick1")
end

function APage:OnApplyViewportResized()
    print("applyviewportresized")
end

function APage:Destruct()
    SuperClass.Destruct(self)
    print("APage Destruct")
end

function APage:OnOpen(luaData, originOpenData)

end

function APage:OnClick1()
    LogInfo(" APage ", "OnClick1");
end


return APage
