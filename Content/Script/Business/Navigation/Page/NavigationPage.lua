local SuperClass = PureMVC.GamePage
local NavigationPage = class("NavigationPage", SuperClass)

-- local NavigationPageMediator = require("Business/Navigation/Mediator/NavigationPageMediator")

function NavigationPage:ListNeededMediators()
    return {
        -- NavigationPageMediator
    }
end

function NavigationPage:Construct()
    SuperClass.Construct(self)
    self:CreateBindEvent("Button_Practice", "OnClicked", "OnPracticeClick")
end

function NavigationPage:Destruct()
    SuperClass.Destruct(self)
end

function NavigationPage:OnPracticeClick()
    print("OnPracticeClick")

    -- UE4.UGameplayStatics.OpenLevel(self, '/Game/FirstPersonCPP/Maps/FirstPersonExampleMap.FirstPersonExampleMap')
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

    local USequenceMapManager = UE4.USequenceMapManager.Get(self)
    USequenceMapManager:TeleportMap(17001)
end

return NavigationPage
