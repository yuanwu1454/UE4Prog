// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/GlobalStateMachine.h"
#include "GlobalPlayingState.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MYGAME_API UGlobalPlayingState : public UGlobalState
{
	GENERATED_BODY()
public:
	virtual void BeginState() override;
	virtual void ConfigEnterStateCoroutine(FCoroutine& PMCoroutine) override;
	virtual void EndState() override;
	virtual void Tick(const float DeltaTime) override;

protected:
	// 在战斗中是否进入了角色展示状态，或者是战斗状态
	// UFUNCTION()
	// void CharacterShowStatusChange(EGamePlayingStage Stage);
	
	UFUNCTION()
	void CheckShowOrHidePageAction(const FName PageName, bool bShow);
	
	UFUNCTION()
	void CheckPageAfterClose(const FName PageName);

	// UFUNCTION()
	// void RetriveGameState(APMGameState* GameState);
	//
	// bool IsPlayingFightStage(EGStateType InStateType) const;
	//
	// UFUNCTION()
	// void OnGameStateChanged(const UCyGMachine* Machine, UCyBaseState* PrevState, UCyBaseState* NextState);
	//
	// void HttpRequestDownloadComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UPROPERTY(EditDefaultsOnly)
	uint32 PracticeMapId = 1002;
	UPROPERTY(EditDefaultsOnly)
	FString PracticeMapName = "Envi_Aim";

	UPROPERTY(EditDefaultsOnly)
	uint32 UGCMapId = 996;
	UPROPERTY(EditDefaultsOnly)
	FString UGCMapName = "UGCMap";
public:
	UPROPERTY(EditDefaultsOnly)
	uint32 TeamGuideMapId = 1005;
	UPROPERTY(EditDefaultsOnly)
	FString TeamGuideMapName = "Envi_Train_TD";

	UPROPERTY(EditDefaultsOnly)
	uint32 BombGuideMapId = 1004;
	UPROPERTY(EditDefaultsOnly)
	FString BombGuideMapName = "Envi_Umeda_bombguide";

	UPROPERTY(EditDefaultsOnly)
	TArray<FString> AIClientGuideMapNames;
private:
	// UPROPERTY(Transient)
	// APMGameState* CachedGameState;
};
