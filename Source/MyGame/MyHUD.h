// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
    // 软引用UI类（蓝图可编辑，路径可配置）
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    FSoftClassPath MainUISoftClass;

	// 异步加载并创建UI（软引用推荐异步加载，避免阻塞主线程）
	UFUNCTION(BlueprintCallable, Category = "UI")
	void AsyncCreateMainUI();
	UFUNCTION()
	void OnWidgetLoaded();
};
