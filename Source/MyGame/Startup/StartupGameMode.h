// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StartupGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AStartupGameMode : public AGameModeBase
{
	GENERATED_UCLASS_BODY()
	virtual void StartPlay() override;
	bool CheckUpdate();
	bool CheckNotice();
	bool CheckFirstPlay();
	void CheckLastLoginServer();

	UFUNCTION()
	void ShowLogin();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> StartupUserWidgetClass;

	UPROPERTY(EditAnywhere)
	bool bUseGlobalState;
};
