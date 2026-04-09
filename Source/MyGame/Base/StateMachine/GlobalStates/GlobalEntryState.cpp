// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalEntryState.h"

#include "Blueprint/UserWidget.h"

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
	if (IsValid(WidgetClass))
	{
		GameStartPageInst = CreateWidget(GetWorld(), WidgetClass);
		if (GameStartPageInst)
		{
			GameStartPageInst->AddToViewport();
			// GameStartPageInst->AnimEndEvent.BindDynamic(this, &UPMGlobalEntryState::OnPresentStudioFinish);
			// GameStartPageInst->StartPresentation();
		}
	}
}
