// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	// 初始化唯一标识（服务器执行）
	virtual void InitializePlayerState(int32 InPlayerId, const FString& InPlayerName = TEXT(""));

	// 获取带ID的唯一名称（如 "Player_01[ID:1]"）
	UFUNCTION(BlueprintCallable)
	FString GetUniquePlayerName() const;

	// 引擎自增唯一ID（服务器分配，同步到客户端）
	UPROPERTY(Replicated)
	int32 UniquePlayerId;

	// 自定义玩家昵称（可选，同步到客户端）
	UPROPERTY(Replicated)
	FString CustomPlayerName;

protected:
	// 声明需要复制的属性（必须！否则客户端拿不到）
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
