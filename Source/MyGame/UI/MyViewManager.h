// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
#include "MyViewControllerPage.h"
#include "MyViewManager.generated.h"

/**
 * 
 */
UCLASS(config=Game)
class MYGAME_API UMyViewManager : public UMyGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<UMyViewControllerPage*> AllPageLst; // 存放所有Page指针,直到退出游戏
	
	// 打开界面
	UMyViewControllerPage* OpenPage(const FName& UIName);
};
