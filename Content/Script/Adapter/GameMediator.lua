local Parent = PureMVC.Mediator
local GameMediator = class("GameMediator", Parent)

function GameMediator:PackNotificationInterests(packlist)
    local list = self.super:ListNotificationInterests()
    local ret = {}
    for i, v in ipairs(packlist) do
        ret[#ret + 1] = v
    end
    for i, v in ipairs(list) do
        ret[#ret + 1] = v
    end
    return ret
end

function GameMediator:ListNotificationInterests()
    return self:PackNotificationInterests({
    })
end

function GameMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
    
end

-- 在公共GameMediator 中使用处理
function GameMediator:OnRegister()
    self.super:OnRegister()
    LogInfo(" GameMediator ", "OnRegister");
end

function GameMediator:OnRemove()
    self.super:OnRemove()
    LogInfo(" GameMediator ", "OnRemove");
end

return GameMediator
