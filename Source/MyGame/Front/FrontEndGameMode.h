// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/MyGameModeBase.h"
#include "FrontEndGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AFrontEndGameMode : public AMyGameModeBase
{
	GENERATED_BODY()
	// 在大厅（FrontEnd）的 GameMode 中空实现 GetSeamlessTravelActorList，是因为大厅场景通常不需要跨关卡保留任何自定义 Actor，遵循 UE 的默认行为即可安全过渡。

	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override {}

	virtual void StartPlay() override;
	virtual void Logout(AController* Exiting) override;
	virtual void StartToLeaveMap() override;

};
