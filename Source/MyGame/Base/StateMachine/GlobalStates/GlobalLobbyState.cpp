// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalLobbyState.h"

#include "MyGameInstance.h"
#include "SubSystem/GlobalDelegateManager.h"

void UGlobalLobbyState::BeginState()
{
	UGlobalDelegateManager* DelegateManager = UGlobalDelegateManager::Get(this);
	if (DelegateManager)
	{
		DelegateManager->OnWaitingLobbyLoadingFinish.AddDynamic(this, &UGlobalLobbyState::StopAutoLoadingClose);
	}
	
	bool bNeedLobbyStreamingLevel = false;
	if (const UGlobalLobbyStateEventData* EvtData = Cast<UGlobalLobbyStateEventData>(GetTriggerEventData()))
	{
		bNeedLobbyStreamingLevel = EvtData->bNeedLobbyStreamingLevel;
	}
	if(!bNeedLobbyStreamingLevel)
	{
		
	}
	
	Super::BeginState();

	if (GetWorld())
	{
		// 为啥 1s 后执行展示tip
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_OpenLobbyTips, FTimerDelegate::CreateUObject(this, &UGlobalLobbyState::ShowOpenLobbyTips), 1.0f, false);
	}
}

void UGlobalLobbyState::EndState()
{
	Super::EndState();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_OpenLobbyTips);
	}
	UGlobalDelegateManager* DelegateManager = UGlobalDelegateManager::Get(this);
	if (DelegateManager)
	{
		DelegateManager->OnWaitingLobbyLoadingFinish.RemoveAll(this);
	}
}

void UGlobalLobbyState::ConfigEnterStateCoroutine(FCoroutine& PMCoroutine)
{
	Super::ConfigEnterStateCoroutine(PMCoroutine);
	Coroutine.ConfigWaitForSeconds(WaitCloseLoadingTime);
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([this]()
	{
		if (!bWaitingLoadingFinish)
		{
			//UPMViewManager::Get(this)->ClosePage(FPMUINames::EM_LoadingAssetPage);

			// if (const auto LoadingStream = UCyLoadingStream::Get(this))
			// {
			// 	LoadingStream->Stop();
			// }
		}
	}));
}

void UGlobalLobbyState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UGlobalLobbyState::StopAutoLoadingClose()
{
	// 告知需要等待
	// 所以loadingstream 不会执行stop 由对应的系统来关闭
	bWaitingLoadingFinish = true;
}

void UGlobalLobbyState::ShowOpenLobbyTips()
{
	//此处只做通用简单提示
	if (UWorld* World = GetWorld())
	{
		if (UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
		{
			if (!GameInstance->OpenLobbyMsgTips.IsEmpty())
			{
				// todo
				// showMsg
				// FCommonPopUpPageData PageData;
				// PageData.bIsOneButton = true;
				// PageData.bNeedChangeInputMode = true;
				// PageData.bCanEscClose = false;
				// PageData.ContentText = GameInstance->OpenLobbyMsgTips;
				// PageData.PageName = FPMUINames::EM_CommonPopUpPage;
				// if (auto PopUpPage = UPMViewManager::Get(World)->OpenPage(FPMUINames::EM_CommonPopUpPage))
				// {
				// 	if( auto PopUpPageView = Cast<UPMCommonPopUpPage>(PopUpPage->GetCacheView()))
				// 	{
				// 		PopUpPageView->SetPageData(PageData);
				// 	}
				// }
			}
			GameInstance->ClearOpenLobbyTips();
		}
		World->GetTimerManager().ClearTimer(TimerHandle_OpenLobbyTips);
	}
}
