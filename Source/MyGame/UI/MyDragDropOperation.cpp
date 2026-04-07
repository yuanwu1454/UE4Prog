// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDragDropOperation.h"

void UMyDragDropOperation::Drop_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Drop_Implementation(PointerEvent);

	// 你自己的拖放放下逻辑
	UE_LOG(LogTemp, Log, TEXT("拖放完成！物品ID：%d"), ItemID);
}

void UMyDragDropOperation::DragCancelled_Implementation(const FPointerEvent& PointerEvent)
{
	Super::DragCancelled_Implementation(PointerEvent);

	// 拖放取消时的逻辑
	UE_LOG(LogTemp, Log, TEXT("拖放被取消"));
}

void UMyDragDropOperation::Dragged_Implementation(const FPointerEvent& PointerEvent)
{
	Super::Dragged_Implementation(PointerEvent);

	// 拖动中每帧执行的逻辑
}