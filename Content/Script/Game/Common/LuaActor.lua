local SuperClass = require("Game/Common/LuaActorBase")
local LuaActor = class('LuaActor', SuperClass)



function LuaActor:ListNeededMediators()
    local mdc = require("Game/LuaActorMediator/StaticMeshComponentMediator")
    return {mdc}
end

function LuaActor:Initialize(Initializer)
    SuperClass.Initialize(self, Initializer)
    LogInfo(" LuaActor ", "Initialize");
end

function LuaActor:UserConstructionScript()
    SuperClass.UserConstructionScript(self)
    LogInfo(" LuaActor ", "UserConstructionScript");
end


function LuaActor:ReceiveBeginPlay()
    SuperClass.ReceiveBeginPlay(self)
    LogInfo(" LuaActor ", "BeginPlay");

    self:DoMediatorByFunc("ReceiveBeginPlay")
    
    self:RefreshTitleName()
    -- 生成组件
    self:CreateDynamicComponents()
    self:StartRefreshWidgetTimer()
end

function LuaActor:ReceiveEndPlay()
    SuperClass.ReceiveEndPlay(self)

    self:DoMediatorByFunc("ReceiveEndPlay")
    LogInfo(" LuaActor ", "EndPlay");
end

-- function LuaActor:ReceiveTick(delta)
--     self.totalDetla = self.totalDetla or 0
--     self.totalDetla = self.totalDetla + delta
--     if self.totalDetla > 0.1 then
--         self.totalDetla  = 0
--         self:RefreshWidget()
--     end
-- end


function LuaActor:CreateDynamicComponents()
    -- 获取组件类引用
    self:InitStaticMeshComponent()
    self:InitParticleComponent()
end

function LuaActor:TestActorComp()
    print("TestActorComp", self.StaticMeshComponent)
    -- print(self:GetWorld():GetTimerManager())
    -- print(self:K2_SetActorRelativeLocation(UE4.FVector(200,0,0), false, UE4.FHitResult(), true))
    -- print(self:K2_AddActorLocalOffset(UE4.FVector(200,0,0), false, UE4.FHitResult(), true))

    -- local arr = self.DefaultSceneRoot:GetAllSocketNames()

    -- print(arr:Length())
    -- for i = 1, arr:Length() do
    --     print(arr:Get(i))
    -- end

    -- local arr = self:K2_GetComponentsByClass(UE4.UActorComponent)
    -- print(arr:Length())
    -- for i = 1, arr:Length() do
    --     print(arr:Get(i))
    -- end
    -- print(self.MeshComponent)
    -- print(self.DefaultSceneRoot)
end


function LuaActor:InitStaticMeshComponent()
    if self.StaticMeshComponent == nil then
        local meshcomp = self:AddComponentByClass(UE4.UStaticMeshComponent, false, UE4.FTransform(), false)
        local CubeMesh = UE4.UStaticMesh.Load('/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube')
        if CubeMesh and meshcomp then
            print("load mesh ...")
            meshcomp:SetStaticMesh(CubeMesh)
        end
    
        meshcomp:K2_AttachToComponent(self.DefaultSceneRoot, "mesh", UE4.EAttachmentRule.KeepRelative, UE4.EAttachmentRule.KeepRelative, UE4.EAttachmentRule.KeepRelative, true)
    
        -- 设置相对位置和缩放
        local hit1 = meshcomp:K2_SetRelativeLocation(UE4.FVector(0, 0, 0), false, UE4.FHitResult(), true)
        self.StaticMeshComponent = meshcomp
        UE4.UMyLuaBridgeBlueprintLibrary.RegisterComponent(meshcomp)
        self.StaticMeshComponent:Activate()
    end
end

function LuaActor:InitParticleComponent()
    if self.ParticleSystemComp == nil then
        local particleSystemComp = self:AddComponentByClass(UE4.UParticleSystemComponent, false, UE4.FTransform(), false)
        local Particle = UE4.LoadObject(ParticleMap.Fire)

        if particleSystemComp and Particle then
            particleSystemComp:SetTemplate(Particle)
            particleSystemComp:K2_AttachToComponent(self.DefaultSceneRoot, "particle", UE4.EAttachmentRule.KeepRelative, UE4.EAttachmentRule.KeepRelative, UE4.EAttachmentRule.KeepRelative, true)
            particleSystemComp:K2_SetRelativeLocation(UE4.FVector(0, 0, 0), false, UE4.FHitResult(), true)
            UE4.UMyLuaBridgeBlueprintLibrary.RegisterComponent(particleSystemComp)
            self.ParticleSystemComp = particleSystemComp
        end
    end
end

function LuaActor:SetStaticMeshByMaterial(materialPath)
    -- FuncUtil:PrintArray(self.StaticMeshComponent:GetMaterialSlotNames() )
    if self.StaticMeshComponent then
        local Material = UE4.LoadObject(materialPath)
        self.StaticMeshComponent:SetMaterialByName("M_Basic_Wall", Material)    
    end

    -- 创建动态材质实例
    -- local MID = MeshComponent:CreateDynamicMaterialInstance(0, nil)  -- 基于插槽 0 的材质创建
    -- -- 设置材质参数（如颜色、数值等）
    -- if MID then
    --     MID:SetVectorParameterValue("Color", UE4.FLinearColor.Red)
    --     MID:SetScalarParameterValue("Glossiness", 0.8)
    --     -- 应用新创建的 MID
    --     MeshComponent:SetMaterial(0, MID)
    -- end
end

-- playercontroller 的 Enable mouse over events 必须勾选上
function LuaActor:ReceiveActorBeginCursorOver()
    SuperClass.ReceiveActorBeginCursorOver(self)
    self:SetStaticMeshByMaterial(MaterialMap.ColorGrid)
end

function LuaActor:ReceiveActorEndCursorOver()
    SuperClass.ReceiveActorEndCursorOver(self)
    self:SetStaticMeshByMaterial(MaterialMap.BasicWall)
end

-- 触发方 一定要带有PrimitiveComponent
-- Object Type 需与目标 Actor 的 Overlap Responses 兼容
-- 目标Actor 一定要带有PrimitiveComponent 作为碰撞体  
-- 且 Generate Overlap Events 必须勾选。
function LuaActor:ReceiveActorBeginOverlap(otherActor)
    SuperClass.ReceiveActorBeginOverlap(self, otherActor)
    LogInfo(" LuaActor ReceiveActorBeginOverlap ", FuncUtil:GetObjectName(otherActor));
    if self.ParticleSystemComp then
        self.ParticleSystemComp:SetVisibility(false, true)
    end
end

function LuaActor:ReceiveActorEndOverlap(otherActor)
    SuperClass.ReceiveActorEndOverlap(self, otherActor)
    LogInfo(" LuaActor ReceiveActorEndOverlap ", FuncUtil:GetObjectName(otherActor));
    if self.ParticleSystemComp then
        self.ParticleSystemComp:SetVisibility(true, true)
    end
end

function LuaActor:ReceiveDestroyed()
    SuperClass.ReceiveDestroyed(self)
    LogInfo(" LuaActor ReceiveDestroyed", FuncUtil:GetObjectName(self));
    
end

-- playercontroller 
-- bEnableClickEvents = true;
-- bEnableMouseOverEvents = true;
-- actor 设置成 block 不可设置成overlap
function LuaActor:ReceiveActorOnClicked(key)
    LogInfo(" LuaActor ReceiveActorOnClicked", FuncUtil:GetKeyName(key));
end

function LuaActor:ReceiveActorOnReleased(key)
    LogInfo(" LuaActor ReceiveActorOnReleased", FuncUtil:GetKeyName(key));
end

function LuaActor:K2_OnBecomeViewTarget(PlayerController)
    LogInfo(" LuaActor K2_OnBecomeViewTarget", FuncUtil:GetObjectName(PlayerController));
end

function LuaActor:K2_OnEndViewTarget(PlayerController)
    LogInfo(" LuaActor K2_OnEndViewTarget", FuncUtil:GetObjectName(PlayerController));
end

function LuaActor:K2_OnReset()
    LogInfo(" LuaActor K2_OnReset", FuncUtil:GetObjectName(self));
end

function LuaActor:ReceiveHit(MyComp, other, othercomp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit)
    -- LogInfo(" LuaActor ReceiveHit",self:GetLuaName())
    -- LogInfo(" LuaActor MyComp", FuncUtil:GetObjectName(MyComp));
    -- LogInfo(" LuaActor other", FuncUtil:GetObjectName(other));
    -- LogInfo(" LuaActor othercomp", FuncUtil:GetObjectName(othercomp));
end

function LuaActor:ReceiveAnyDamage(Damage, DamageType, InstigatedBy, DamageCauser)
    print(self:GetLuaName())
    print(Damage, DamageType, FuncUtil:GetObjectName(InstigatedBy), FuncUtil:GetObjectName(DamageCauser))
end

function LuaActor:RefreshWidget()
    if self.WidgetComponent then
        local playerController = LuaGetPlayerController()
        local cameraLocation = UE4.FVector()
        local cameraRotation = UE4.FRotator() 
        playerController:GetActorEyesViewPoint(cameraLocation, cameraRotation)

        local componentLocation = self.WidgetComponent:K2_GetComponentLocation()
        local direction = cameraLocation - componentLocation

        local direction_normal = UE4.UKismetMathLibrary.Normal(direction);
        local rotator = UE4.UKismetMathLibrary.MakeRotFromX(direction_normal);
        self.WidgetComponent:K2_SetWorldRotation(rotator, false, UE4.FHitResult(), true)
    end
    
end

function LuaActor:StartRefreshWidgetTimer()
    self:AddPeriodTimeTask(0.1, function ()
        self:RefreshWidget()
    end, "RefreshWidgetTag")
end

function LuaActor:RefreshTitleName()
    if self.luaName then
        local widget = self.WidgetComponent:GetWidget()
        if widget and widget.TextBlock_Name then
            widget.TextBlock_Name:SetText(self.luaName)
        end
    end
end

function LuaActor:SetLuaName(name)
    SuperClass.SetLuaName(self, name)
    self:RefreshTitleName()
end


return LuaActor