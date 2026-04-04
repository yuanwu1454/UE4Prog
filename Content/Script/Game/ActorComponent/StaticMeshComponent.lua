local SuperClass = require("Game.ActorComponent.ActorComponentBase")
local StaticMeshComponent = class('StaticMeshComponent', SuperClass)

function StaticMeshComponent:ReceiveBeginPlay()
    LogInfo(" StaticMeshComponent ", tostring("ReceiveBeginPlay"));
end

function StaticMeshComponent:ReceiveEndPlay()
    LogInfo(" StaticMeshComponent ", tostring("ReceiveEndPlay"));
end

-- function StaticMeshComponent:OnComponentBeginOverlap(overlapComp, otherActor, otherComp, otherBodyIndex, bSweep, hitResult)
--     LogInfo(" StaticMeshComponent ", tostring("OnComponentBeginOverlap") .. FuncUtil:GetObjectName(overlapComp)
--     .. FuncUtil:GetObjectName(otherActor)
--     .. FuncUtil:GetObjectName(otherComp));
-- end

function StaticMeshComponent:SetOnComponentBeginOverlapFunc(func, obj)
    print("x", self.OnComponentBeginOverlap.Add)
    self.OnComponentBeginOverlap:Add(obj, func)
    -- self.OnComponentBeginOverlap:Add(funcname, obj)
end

-- function StaticMeshComponent:LuaComponentEventBeginOverlap()
-- end

return StaticMeshComponent

-- Begin Object Class=/Script/BlueprintGraph.K2Node_ComponentBoundEvent Name="K2Node_ComponentBoundEvent_1"
--    DelegatePropertyName="OnBeginCursorOver"
--    DelegateOwnerClass=Class'"/Script/Engine.PrimitiveComponent"'
--    ComponentPropertyName="StaticMeshComponent"
--    DelegatePropertyDisplayName=NSLOCTEXT("UObjectDisplayNames", "PrimitiveComponent:OnBeginCursorOver", "On Begin Cursor Over")
--    EventReference=(MemberParent=Package'"/Script/Engine"',MemberName="ComponentBeginCursorOverSignature__DelegateSignature")
--    bInternalEvent=True
--    CustomFunctionName="BndEvt__LuaActor_StaticMeshComponent_K2Node_ComponentBoundEvent_1_ComponentBeginCursorOverSignature__DelegateSignature"
--    NodePosX=-160
--    NodePosY=912
--    NodeGuid=021F50F14F535AD554BEE8B55A814286
--    CustomProperties Pin (PinId=549B8390458988E5B133A0ACEA25B2BC,PinName="OutputDelegate",Direction="EGPD_Output",PinType.PinCategory="delegate",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
--    CustomProperties Pin (PinId=64F43F9340824F6B69AC3FBA6BDAD11F,PinName="then",Direction="EGPD_Output",PinType.PinCategory="exec",PinType.PinSubCategory="",PinType.PinSubCategoryObject=None,PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
--    CustomProperties Pin (PinId=F58D7F6C4F20E257B8C09FAD7264F0B6,PinName="TouchedComponent",PinToolTip="Touched Component\nPrimitive Component Object Reference",Direction="EGPD_Output",PinType.PinCategory="object",PinType.PinSubCategory="",PinType.PinSubCategoryObject=Class'"/Script/Engine.PrimitiveComponent"',PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
-- End Object

