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
};
