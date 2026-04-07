// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "MyDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	// ==============================================
	// 【你可以在这里加自定义数据！核心用途！】
	// ==============================================
	UPROPERTY(BlueprintReadWrite, Category = "自定义拖放数据")
	int32 ItemID = 0;

	UPROPERTY(BlueprintReadWrite, Category = "自定义拖放数据")
	FString ItemName;
	
	// ==============================================
	// 【重写拖放事件】
	// ==============================================
	virtual void Drop_Implementation(const FPointerEvent& PointerEvent) override;
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;
	virtual void Dragged_Implementation(const FPointerEvent& PointerEvent) override;
};
