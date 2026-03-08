// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// 初始化输入绑定（PlayerController 专属）
	virtual void SetupInputComponent() override;

	// 按键 Q 触发的函数（客户端执行）
	UFUNCTION()
	void OnTriggerQPressed();

private:
	// 获取当前操控的 PlayerCharacter
	class AMyPlayerCharacter* GetControlledCharacter();
};
