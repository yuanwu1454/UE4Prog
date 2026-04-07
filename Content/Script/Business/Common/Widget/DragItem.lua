-- ##requires
local SuperClass = PureMVC.GamePanel
local DragItem = class("DragItem", SuperClass)

function DragItem:Construct()
    print("DragItem Construct")
    SuperClass.Construct(self)
end

function DragItem:Destruct()
    print("DragItem Destruct")
    SuperClass.Destruct(self)
end



function DragItem:OnDragDetected(MyGeometry, MouseEvent, Operation)
    local OperationClass = FuncUtil:LoadClass('/Game/UI/MyDragDropOperation_BP.MyDragDropOperation_BP')
    local dropOperation = UE4.UWidgetBlueprintLibrary.CreateDragDropOperation(OperationClass)
    dropOperation.Pivot = UE4.EDragPivot.CenterCenter
    dropOperation.DefaultDragVisual = self.Image_Drop
    dropOperation.Payload = self
    dropOperation.OnDrop:Add(self, self.OnDragDrop_Success)
    dropOperation.OnDragCancelled:Add(self, self.OnDragDrop_Cancel)
    dropOperation.OnDragged:Add(self, self.OnDragDrop_Dragged)
    return dropOperation
end

function DragItem:OnDragCancelled(Operation)
    print("DragItem OnDragCancelled")
end

function DragItem:OnDragEnter(MyGeometry, PointerEvent, Operation)
    print("DragItem OnDragEnter")
    print("Operation ItemID ", Operation.ItemID)
end

function DragItem:OnDragLeave(PointerEvent, Operation)
    print("DragItem OnDragLeave")
    print("Operation ItemID ", Operation.ItemID)
end

function DragItem:OnDragOver(MyGeometry, PointerEvent, Operation)
    print("DragItem OnDragOver")
    -- 返回true 表示告诉引擎 不需要处理了 
    -- 返回false 表示告诉引擎 要处理了
    return true
end

function DragItem:OnDrop(MyGeometry, PointerEvent, Operation)
    print("DragItem OnDrop")
    -- 返回true 表示告诉引擎 不需要处理了 
    -- 返回false 表示告诉引擎 要处理了
    return true
end

function DragItem:OnDragDrop_Success(Operation)
    print("DragItem OnDragDrop_Success")
end

function DragItem:OnDragDrop_Cancel(Operation)
    print("DragItem OnDragDrop_Cancel")
end

function DragItem:OnDragDrop_Dragged(Operation)
    -- print("DragItem OnDragDrop_Dragged")
end



function DragItem:OnMouseButtonDown(MyGeometry, MouseEvent)
    print("DragItem OnMouseButtonDown")
   -- 【必须】定义鼠标左键
    local mouseKey = UE4.FKey()
    mouseKey.KeyName = "LeftMouseButton"  -- 关键！你之前漏了这个！

    return UE4.UWidgetBlueprintLibrary.DetectDragIfPressed(MouseEvent,self,mouseKey)
end

function DragItem:OnMouseButtonUp(MyGeometry, MouseEvent)
    print("DragItem OnMouseButtonUp")
    return UE4.UWidgetBlueprintLibrary.Handled()
end


-- Begin Object Class=/Script/BlueprintGraph.K2Node_CallFunction Name="K2Node_CallFunction_1"
--    bIsPureFunc=True
--    FunctionReference=(MemberParent=Class'"/Script/UMG.WidgetBlueprintLibrary"',MemberName="CaptureMouse")
--    NodePosX=480
--    NodePosY=160
--    NodeGuid=7EFF39BF431306F3A41B3B8CDC4CEFD6
--    CustomProperties Pin (PinId=F40A09AB4AC8632535C7C197D7A8B995,PinName="self",PinFriendlyName=NSLOCTEXT("K2Node", "Target", "Target"),PinToolTip="Target\nWidget Blueprint Library Object Reference",PinType.PinCategory="object",PinType.PinSubCategory="",PinType.PinSubCategoryObject=Class'"/Script/UMG.WidgetBlueprintLibrary"',PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,DefaultObject="/Script/UMG.Default__WidgetBlueprintLibrary",PersistentGuid=00000000000000000000000000000000,bHidden=True,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
--    CustomProperties Pin (PinId=704E14E148F1CF7017D95FB7866AACB6,PinName="Reply",PinToolTip="Reply\nEvent Reply Structure (by ref)",PinType.PinCategory="struct",PinType.PinSubCategory="",PinType.PinSubCategoryObject=ScriptStruct'"/Script/UMG.EventReply"',PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=True,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,LinkedTo=(K2Node_CallFunction_0 9F7B7A7B4C45AF20BC1BEBB3A05F154A,),PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
--    CustomProperties Pin (PinId=1E082C42450CBD55D2A5108CEF923800,PinName="CapturingWidget",PinToolTip="Capturing Widget\nWidget Object Reference",PinType.PinCategory="object",PinType.PinSubCategory="",PinType.PinSubCategoryObject=Class'"/Script/UMG.Widget"',PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
--    CustomProperties Pin (PinId=7B7965BE458DE4193BAE65B8DABF64EF,PinName="ReturnValue",PinToolTip="Return Value\nEvent Reply Structure\n\nCapture Mouse",Direction="EGPD_Output",PinType.PinCategory="struct",PinType.PinSubCategory="",PinType.PinSubCategoryObject=ScriptStruct'"/Script/UMG.EventReply"',PinType.PinSubCategoryMemberReference=(),PinType.PinValueType=(),PinType.ContainerType=None,PinType.bIsReference=False,PinType.bIsConst=False,PinType.bIsWeakPointer=False,PinType.bIsUObjectWrapper=False,PersistentGuid=00000000000000000000000000000000,bHidden=False,bNotConnectable=False,bDefaultValueIsReadOnly=False,bDefaultValueIsIgnored=False,bAdvancedView=False,bOrphanedPin=False,)
-- End Object





return DragItem
