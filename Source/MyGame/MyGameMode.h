// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameSession.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
public:
	// 重写：指定使用自定义GameSession
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void GameWelcomePlayer(UNetConnection* Connection, FString& RedirectURL) override;
	virtual void Logout(AController* Exiting) override;
	virtual void InitGameState() override;
private:
	// 延迟重试获取玩家的计时器
	FTimerHandle RetryGetPlayerTimer;
	// 最大重试次数（避免无限重试）
	int32 MaxRetryCount = 5;
	int32 CurrentRetryCount = 0;
};

