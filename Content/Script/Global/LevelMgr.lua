local LevelMgr = class("LevelMgr")


function LevelMgr:ctor()
    print("LevelMgr:ctor")
end

function LevelMgr:openLevel(levelName, bAbsolute)
    if bAbsolute == nil then
        bAbsolute = true
    end
    UE4.UGameplayStatics.OpenLevel(self, levelName, bAbsolute)
end

_G.LevelMgr = LevelMgr.new()


--  bAbsolute = true（绝对模式）
-- 含义：完全忽略当前游戏的 URL / 路径上下文，直接把 LevelName 当作完整、独立的目标地址去加载。
-- 行为：
-- 清空当前的连接 / 路径上下文
-- 直接使用你给的关卡名 / URL 作为最终目标
-- 适合：跳转到全新独立关卡、切换主地图、网络独立切换场景
-- 源码对应：TRAVEL_Absolute