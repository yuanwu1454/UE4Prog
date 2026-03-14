// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "MyGame/BasePlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyPlayerController : public ABasePlayerController
{
	GENERATED_BODY()
public:
	// 初始化输入绑定（PlayerController 专属）
	virtual void SetupInputComponent() override;

	// 按键 Q 触发的函数（客户端执行）
	UFUNCTION()
	void OnTriggerQPressed();
	virtual void OnPossess(APawn* aPawn) override;

	virtual void OnRep_Pawn();
	FString GetPlayerName() const;
private:
	// 获取当前操控的 PlayerCharacter
	AMyPlayerCharacter* GetControlledCharacter();
};
