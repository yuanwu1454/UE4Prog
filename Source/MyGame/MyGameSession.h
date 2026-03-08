// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameSession.generated.h"

UCLASS()
class MYGAME_API AMyGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyGameSession();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// 重写原生AGameSession的核心回调（这些是真正存在的可重写方法）
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void NotifyLogout(const APlayerController* PC) override;
	virtual bool AtCapacity(bool bSpectator) override;
	
	// 供GameMode调用的销毁Session接口（设为public）
	bool DestroySession(FName InSessionName);
	// 销毁完成的回调（设为public，供GameMode绑定/监听，可选）
	void OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful);

	
	bool CreateSession(FName InSessionName, int32 MaxPlayers = 4);
	// MyGameSession.h 中补充声明（public 权限，供 GameMode 调用）
	void OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful);
	
	// MyGameSession.h 中补充声明
	// 供GameMode调用：查找可用Session（客户端）
	bool FindAvailableSessions();
	void OnFindSessionsComplete(bool bWasSuccessful); // 查找Session完成回调
	
	// 供GameMode调用：加入指定Session（客户端）
	bool JoinGameSession(FName SessionName, const FOnlineSessionSearchResult& SessionResult);
	void OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result); // 加入Session完成回调

	// 服务器判断逻辑
	bool IsServer() const;
	bool IsPureClient() const;
	
private:


	// 辅助方法：获取当前GameMode（安全封装）
	AGameModeBase* GetCurrentGameMode() const;
	// 辅助方法：获取当前在线玩家数（替代原错误的GetNumPlayers()）
	int32 GetCurrentPlayerCount() const;
	// 辅助方法：获取当前观众数（替代原错误的GetNumSpectators()）
	int32 GetCurrentSpectatorCount() const;


	// 重置Session状态
	void ResetSessionState();
	bool InitSessionInterface();
	// Session接口指针
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch; // 会话查找结果缓存
	bool GetValidPlayerNetId(FUniqueNetIdRepl& OutNetId, const FName& InSessionName);
};
