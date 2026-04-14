// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorBase.generated.h"

UCLASS()
class MYGAME_API AActorBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorBase();

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Helper")
	class AHUD* GetHUD() const;

	// 获取 玩家控制器
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Helper")
	class APlayerController* GetPlayerController() const;

	// 获取 玩家状态（PlayerState）
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Helper")
	class APlayerState* GetPlayerState() const;

	// 获取 GameMode
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Helper")
	class AGameModeBase* GetGameMode() const;

	// 获取 GameState
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Helper")
	class AGameStateBase* GetGameState() const;
};
