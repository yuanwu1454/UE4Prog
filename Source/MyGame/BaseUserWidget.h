// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	// ========== 通用UI辅助函数 ==========

	/**
	 * 恢复到游戏输入模式
	 */
	void RestoreToGameMode();

	/**
	 * 安全获取当前玩家控制器
	 * 避免空指针报错
	 */
	APlayerController* GetSafePlayerController();

	/**
	 * 关闭当前UI
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Base")
	void CloseUI();

	/**
	 * 获取自身所在的World
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Base")
	UWorld* GetSelfWorld();
};