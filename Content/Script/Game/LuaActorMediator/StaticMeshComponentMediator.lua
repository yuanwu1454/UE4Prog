local StaticMeshComponentMediator = class("StaticMeshComponentMediator", PureMVC.Mediator)
function StaticMeshComponentMediator:ListNotificationInterests()
    return {
        
    }
end

function StaticMeshComponentMediator:HandleNotification(notification)
    local name = notification:GetName()
    local body = notification:GetBody()
end

function StaticMeshComponentMediator:OnRegister()
    PureMVC.Mediator.OnRegister(self)
    LogInfo("StaticMeshComponentMediator", "OnRegister")

    LogInfo(" StaticMeshComponentMediator ", tostring(self:GetViewComponent()).. tostring(self:GetViewComponent()));
    local tbl = self:GetViewComponent()
    table.print(tbl)
    
    self:GetViewComponent():AddDelayTimeTask(0.5, function ()
        local tbl = self:GetViewComponent()
        table.print(tbl)
    end, "DelayTag")
end

function StaticMeshComponentMediator:ReceiveBeginPlay()
end

function StaticMeshComponentMediator:ReceiveEndPlay()
end

function StaticMeshComponentMediator:OnRemove()
    PureMVC.Mediator.OnRemove(self)
    LogInfo("StaticMeshComponentMediator", "OnRemove")
end


return StaticMeshComponentMediator

