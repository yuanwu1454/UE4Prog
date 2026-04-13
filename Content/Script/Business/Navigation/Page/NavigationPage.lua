local SuperClass = require("Business/Common/Tab/TabSwitchPage")
local NavigationPage = class("NavigationPage", SuperClass)

function NavigationPage:ListNeededMediators()
    return {
        -- NavigationPageMediator
    }
end

function NavigationPage:Construct()
    SuperClass.Construct(self)
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

    -- local USequenceMapManager = UE4.USequenceMapManager.Get(self)
    -- USequenceMapManager:TeleportMap(17001)

    ShowTip("这是导航页面")
end

function NavigationPage:OnOpen(luaOpenData, nativeOpenData)
	SuperClass.OnOpen(self, luaOpenData, nativeOpenData)
end

-- override 获取各个tab的信息
function NavigationPage:GetTabInfos()

    if self.tabInfos == nil then
        self.tabInfos = {}
        table.insert(self.tabInfos, {name = "A", idx_name = "A"})
        table.insert(self.tabInfos, {name = "B", idx_name = "B"})
        for i, v in ipairs(self.tabInfos) do
            v.id = i
        end
    end


    return self.tabInfos
end


-- override 根据id 获取page class
function NavigationPage:GetPageClass(id)
    local name = nil
    for _, v in ipairs(self.tabInfos) do
        if v.id == id then
            name = v.idx_name
            break
        end
    end
    if name then
        local pageClass = self.PageClassMap:Find(name)
        return pageClass
    end
end

function NavigationPage:GetLastOpenData()
    return 
end

-- override 根据传入的defaultid 与 自己tabInfo 判定是否相等
function NavigationPage:Cmp(tabInfo, id)
	if tabInfo and tabInfo.id and id then
		return tabInfo.id == id
	end
	return false
end

function NavigationPage:GetIndex(id)
    for i, v in ipairs(self.tabInfos) do
        if self:Cmp(v, id) then
            return i
        end
    end
    return 1
end


-- override 根据传入的index 获取 应该得到的id， 这个id 是唯一标识。
function NavigationPage:GetID(index)
    if self.tabInfos[index] then
        return self.tabInfos[index].id
    end
    return index
end

return NavigationPage
