// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MyTableRowBase.h"
#include "UICommon.h"
#include "Widgets/Layout/Anchors.h"
#include "MyViewController.h"
#include "MyUITableRow.generated.h"

USTRUCT(BlueprintType)
struct FMyUITableRow : public FMyTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PageName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMyViewController> PageClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MetaClass = "UserWidget"))
	FSoftClassPath ViewClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsPanel == false"))
	EUILayer Layer = EUILayer::Normal;

	//导航栏ZOrder为20,导航栏下方的Page为0-20,新界面要高于20
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsPanel == false"))
	uint8 Order;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsPanel == false"))
	bool FullScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsPanel == false"))
	bool OnlyScreen2DUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bIsPanel))
	TArray<FName> SlotNameList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bIsPanel))
	FAnchors Anchors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bIsPanel))
	FVector2D Alignment;
};