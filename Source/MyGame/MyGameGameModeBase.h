// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameGameModeBase();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
public:
	// 核心：兼容单机/服务器的玩家获取函数
	UFUNCTION(BlueprintCallable, Category="PlayerUtils")
	AMyPlayerCharacter* GetValidPlayerCharacter(bool bForceLocal = false);

	// 示例：初始化玩家属性（兼容两种模式）
	UFUNCTION(BlueprintCallable, Category="PlayerUtils")
	void InitPlayerAttributes(float NewSpeed = 600.0f, int32 NewFPS = 60);

private:
	// 延迟重试获取玩家的计时器
	FTimerHandle RetryGetPlayerTimer;
	// 最大重试次数（避免无限重试）
	int32 MaxRetryCount = 5;
	int32 CurrentRetryCount = 0;
};
