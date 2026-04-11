// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/HUDBase.h"
#include "FrontEndHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AFrontEndHUD : public AHUDBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; 
protected:
	UPROPERTY()
	float WaitTimeBeforeCloseLoading = 1.0f;
};
