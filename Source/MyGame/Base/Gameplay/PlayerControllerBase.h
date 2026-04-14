// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void EnterGameOnlyInputMode();

	UFUNCTION(BlueprintCallable)
	void EnterUIOnlyInputMode();

	// 额外补上：HUD HitBox 必须用的模式（最重要！）
    UFUNCTION(BlueprintCallable, Category = "Input Mode")
    void EnterGameAndUIInputMode();
    

	// 必须用 UFUNCTION(exec) ！！
	// 不能带 BlueprintCallable（UE4.27 不兼容）
	UFUNCTION(exec)
	void GameOnly();

	UFUNCTION(exec)
	void UIOnly();

	UFUNCTION(exec)
	void GameAndUI();
};
