// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/GlobalStateMachine.h"
#include "GlobalLobbyState.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MYGAME_API UGlobalLobbyState : public UGlobalState
{
	GENERATED_BODY()
public:
	virtual void BeginState() override;
	virtual void EndState() override;
	virtual void ConfigEnterStateCoroutine(FCoroutine& PMCoroutine) override;
	virtual void Tick(const float DeltaTime) override;
private:
	UFUNCTION()
	void StopAutoLoadingClose();
	UFUNCTION()
	void ShowOpenLobbyTips();
	// 如果在加载过程中收到此消息则由对应的模块来决定关闭Loading
	bool bWaitingLoadingFinish = false;

	UPROPERTY(EditDefaultsOnly)
	float WaitCloseLoadingTime = 0.5f;

	FTimerHandle TimerHandle_OpenLobbyTips;
};
