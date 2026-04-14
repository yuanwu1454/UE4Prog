// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorBase.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

// Sets default values
AActorBase::AActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

AHUD* AActorBase::GetHUD() const
{
	// 1. 获取玩家控制器
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return nullptr;

	// 2. 返回 HUD
	return PC->GetHUD();
}


// 获取 PlayerController
APlayerController* AActorBase::GetPlayerController() const
{
	return UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

// 获取 PlayerState
APlayerState* AActorBase::GetPlayerState() const
{
	APlayerController* PC = GetPlayerController();
	return PC ? PC->PlayerState : nullptr;
}

// 获取 GameMode
AGameModeBase* AActorBase::GetGameMode() const
{
	return UGameplayStatics::GetGameMode(GetWorld());
}

// 获取 GameState
AGameStateBase* AActorBase::GetGameState() const
{
	return UGameplayStatics::GetGameState(GetWorld());
}