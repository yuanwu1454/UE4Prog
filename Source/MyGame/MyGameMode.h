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
	// 核心：兼容单机/服务器的玩家获取函数
	UFUNCTION(BlueprintCallable, Category="PlayerUtils")
	AMyPlayerCharacter* GetValidPlayerCharacter(bool bForceLocal = false);

	// 示例：初始化玩家属性（兼容两种模式）
	UFUNCTION(BlueprintCallable, Category="PlayerUtils")
	void InitPlayerAttributes(float NewSpeed = 600.0f, int32 NewFPS = 60);


	// 重写：指定使用自定义GameSession
	virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

	// // 蓝图可调用：创建会话（对外暴露的入口）
	// UFUNCTION(BlueprintCallable, Category = "Session")
	// bool CreateGameSession(FName SessionName = FName("MyGameSession"), int32 MaxPlayers = 4);
	//
	// // 加入游戏只能通过rpc来让服务器完成。
	// // MyGameMode.h 中补充声明
	// UFUNCTION(BlueprintCallable, Category = "Session")
	// bool FindGameSessions(); // 客户端查找可用Session
	// bool JoinGameSession(FName SessionName, const FOnlineSessionSearchResult& SessionResult); 
private:
	// 延迟重试获取玩家的计时器
	FTimerHandle RetryGetPlayerTimer;
	// 最大重试次数（避免无限重试）
	int32 MaxRetryCount = 5;
	int32 CurrentRetryCount = 0;
};
