// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerState.h"

#include "Net/UnrealNetwork.h"

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 注册需要同步的属性
	DOREPLIFETIME(AMyPlayerState, UniquePlayerId);
	DOREPLIFETIME(AMyPlayerState, CustomPlayerName);
}

void AMyPlayerState::InitializePlayerState(int32 InPlayerId, const FString& InPlayerName)
{
	if (!HasAuthority()) return; // 仅服务器初始化
	UniquePlayerId = InPlayerId;
	CustomPlayerName = InPlayerName.IsEmpty() ? FString::Printf(TEXT("Player_%02d"), InPlayerId) : InPlayerName;
}

FString AMyPlayerState::GetUniquePlayerName() const
{
	return FString::Printf(TEXT("%s[ID:%d]"), *CustomPlayerName, UniquePlayerId);
}