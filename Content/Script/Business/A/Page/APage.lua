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
    -- UE4.UGameplayStatics.OpenLevel(self, '/Game/FirstPersonCPP/Maps/FirstPersonExampleMap', true, "a=1")

    UE4.UGameplayStatics.OpenLevel(self, "/Game/Cinema/King", true)
    -- 你用了带点后缀的写法，等于把 “主地图切换” 当成了 “流关卡加载”，
    -- 2. 错误写法（你不能动）
    -- plaintext
    -- /Game/FirstPersonCPP/Maps/FirstPersonExampleMap.FirstPersonExampleMap
    -- → 打开的是：地图内部的 子关卡（Level Streaming）
    -- → 触发：流关卡加载流程，不是完整地图切换！
    -- 💀 后果就是：
    -- 不会重新创建 PlayerController
    -- 不会重新创建 Character
    -- 不会重新初始化 MovementComponent
    -- SetUpdatedComponent 不执行
    -- PawnOwner = nullptr
    -- 角色有输入，但是不动！
end


return APage
