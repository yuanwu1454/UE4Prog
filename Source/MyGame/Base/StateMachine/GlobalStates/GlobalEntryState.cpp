// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalEntryState.h"

#include "MyGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "StartUp/GameStartPage.h"

void UGlobalEntryState::ConfigEnterStateCoroutine(FCoroutine& PMCoroutine)
{
	Super::ConfigEnterStateCoroutine(PMCoroutine);

	if (bSkip)
	{
		Coroutine.ConfigNewAction(FCoroutineAction::CreateUObject(this, &UGlobalEntryState::TransferToNextState));		
	}else
	{
		ConfigShowStudioPresent();
		// 进入初始化状态
		Coroutine.ConfigNewAction(FCoroutineAction::CreateUObject(this, &UGlobalEntryState::TransferToNextState));		
	}

}

void UGlobalEntryState::ConfigShowStudioPresent()
{
	const UGlobalEntryStateEventData* EventData = Cast<UGlobalEntryStateEventData>(GetTriggerEventData());
	if (EventData)
	{
		// 播放工作室动画
		Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([this, EventData]()
		{
			TSubclassOf<UUserWidget> StartupUserWidget = StartupUserWidgetClass;
			auto GI = GetWorld()->GetGameInstance<UMyGameInstance>();
			CreatePresentAnimWidget(StartupUserWidget);
		}));
		// 等待播放完成
		Coroutine.ConfigWaitAction(FCoroutineWaitAction::CreateUObject(this, &UGlobalEntryState::CheckShowStudioPresentFinish));
	}
}

void UGlobalEntryState::OnPresentStudioFinish()
{
	bStudioWidgetPresentFinish = true;
}

bool UGlobalEntryState::CheckShowStudioPresentFinish() const
{
	return bStudioWidgetPresentFinish;
}

void UGlobalEntryState::TransferToNextState()
{
	UGlobalResUpStateEventData* Data = NewObject<UGlobalResUpStateEventData>();
	Data->DebugMessage = TEXT("Entry->Resup");
	Data->ResUpReason = EResUpReasonType::FirstResourceUpdate;
	Data->PtrGameStartPageInst = GameStartPageInst;
	TransferState(EGlobalStateType::ResUp, Data);
}

void UGlobalEntryState::CreatePresentAnimWidget(UClass* WidgetClass)
{
	if (IsValid(WidgetClass))
	{
		GameStartPageInst = CreateWidget(GetWorld(), WidgetClass);
		if (GameStartPageInst)
		{
			GameStartPageInst->AddToViewport();
			auto StartupPage = Cast<UGameStartPage>(GameStartPageInst);
			StartupPage->AnimEndEvent.BindDynamic(this, &UGlobalEntryState::OnPresentStudioFinish);
			StartupPage->StartPresentation();
		}
	}
}
