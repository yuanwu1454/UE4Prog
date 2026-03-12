// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// 初始化输入绑定（PlayerController 专属）
	virtual void SetupInputComponent() override;

	// 按键 Q 触发的函数（客户端执行）
	UFUNCTION()
	void OnTriggerQPressed();
	// 客户端调用：触发加入指定Session
	UFUNCTION(BlueprintCallable, Category = "Session")
	void Client_RequestJoinSpecificSession(const FString& TargetSessionName);

	// 服务器RPC：查询GameMode中的Session，并返回给客户端
	UFUNCTION(Server, Reliable)
	void Server_GetSessionInfo(const FString& TargetSessionName);
	void Server_GetSessionInfo_Implementation(const FString& TargetSessionName);

	// 客户端回调：服务器返回Session信息后，本地执行加入
	UFUNCTION(Client, Reliable)
	void Client_OnSessionInfoReceived(const TArray<FString>& SessionResult);
	void Client_OnSessionInfoReceived_Implementation(const TArray<FString>& SessionResult);
private:
	// 获取当前操控的 PlayerCharacter
	AMyPlayerCharacter* GetControlledCharacter();
};
