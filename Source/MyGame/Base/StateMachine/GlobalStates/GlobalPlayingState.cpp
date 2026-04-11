// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalPlayingState.h"

#include "SubSystem/GlobalDelegateManager.h"

void UGlobalPlayingState::BeginState()
{
	UGlobalDelegateManager* GlobalDelMgr = UGlobalDelegateManager::Get(this);
	if (const UGlobalPlayingStateEventData* EventData = Cast<UGlobalPlayingStateEventData>(GetTriggerEventData()))
	{
		// 展示Loading
		uint32 MapId;
		switch(EventData->MapType)
		{
		case EPlayingMapType::Practice_Map:
			MapId = PracticeMapId;
			break;
		case EPlayingMapType::TeamGuide_Map:
			MapId = TeamGuideMapId;
			break;
		case EPlayingMapType::BombGuide_Map:
			MapId = BombGuideMapId;
			break;
		case EPlayingMapType::UGC_Map:
			MapId = UGCMapId;
			break;
		default:
			MapId = EventData->MapId;
			break;
		}
		if (GlobalDelMgr)
		{
			GlobalDelMgr->OpenWillReconnectToMap.Broadcast(MapId, EventData->MapUrl);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Try force enter playing state"));
	}
	if (GlobalDelMgr)
	{
		// GlobalDelMgr->OnPlayingShowStageChange.AddDynamic(this, &UGlobalPlayingState::CharacterShowStatusChange);
		// GlobalDelMgr->OnViewPageShowOrHide.AddDynamic(this, &UGlobalPlayingState::CheckShowOrHidePageAction);
		// GlobalDelMgr->OnViewPageAfterClose.AddDynamic(this, &UGlobalPlayingState::CheckPageAfterClose);
		// GlobalDelMgr->OnPlayingGameStateBeginPlay.AddDynamic(this, &UGlobalPlayingState::RetriveGameState);
	}
	
	Super::BeginState();
}

void UGlobalPlayingState::ConfigEnterStateCoroutine(FCoroutine& PMCoroutine)
{
	Super::ConfigEnterStateCoroutine(PMCoroutine);
}

void UGlobalPlayingState::EndState()
{
	Super::EndState();
}

void UGlobalPlayingState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// void UGlobalPlayingState::CharacterShowStatusChange(EGamePlayingStage Stage)
// {
// }

void UGlobalPlayingState::CheckShowOrHidePageAction(const FName PageName, bool bShow)
{
}

void UGlobalPlayingState::CheckPageAfterClose(const FName PageName)
{
}
//
// void UGlobalPlayingState::RetriveGameState(APMGameState* GameState)
// {
// }
//
// bool UGlobalPlayingState::IsPlayingFightStage(EGStateType InStateType) const
// {
// }
//
// void UGlobalPlayingState::OnGameStateChanged(const UCyGMachine* Machine, UCyBaseState* PrevState,
// 	UCyBaseState* NextState)
// {
// }
//
// void UGlobalPlayingState::HttpRequestDownloadComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
// 	bool bSucceeded)
// {
// }
