// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalEntryState.h"

void UGlobalEntryState::ConfigEnterStateCoroutine(FCoroutine& PMCoroutine)
{
	Super::ConfigEnterStateCoroutine(PMCoroutine);
	
	// 进入初始化状态
	Coroutine.ConfigNewAction(FCoroutineAction::CreateUObject(this, &UGlobalEntryState::TransferToNextState));
}

void UGlobalEntryState::ConfigShowStudioPresent()
{
}

void UGlobalEntryState::OnPresentStudioFinish()
{
}

bool UGlobalEntryState::CheckShowStudioPresentFinish() const
{
	return false;
}

void UGlobalEntryState::TransferToNextState()
{
}

void UGlobalEntryState::CreatePresentAnimWidget(UClass* WidgetClass)
{
	UGlobalResUpStateEventData* Data = NewObject<UGlobalResUpStateEventData>();
	// Data->DebugMessage = TEXT("Entry->Resup");
	// Data->ResUpReason = EResUpReasonType::FirstResourceUpdate;
	// Data->PtrGameStartPageInst = GameStartPageInst;
	// TransferState(EPMGlobalStateType::ResUp, Data);
}
