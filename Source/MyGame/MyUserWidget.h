// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	// ========== 辅助函数 ==========
	/**
	 * 切换到UI输入模式（游戏+UI 或 仅UI）
	 */
	void SwitchToUIMode();

	/**
	 * 恢复到游戏输入模式
	 */
	void RestoreToGameMode();

	/**
	 * 安全获取当前玩家控制器
	 * 避免空指针报错
	 */
	APlayerController* GetSafePlayerController();
};