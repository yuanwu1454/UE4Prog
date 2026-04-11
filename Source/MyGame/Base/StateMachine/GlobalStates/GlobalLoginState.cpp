// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalLoginState.h"

#include "BaseUserWidget.h"
#include "MyViewManager.h"
#include "Types.h"
#include "StartUp/GameStartPage.h"

void UGlobalLoginState::BeginState()
{
	Super::BeginState();
}

void UGlobalLoginState::EndState()
{
	Super::EndState();
}

void UGlobalLoginState::ConfigEnterStateCoroutine(FCoroutine& PMCoroutine)
{
	Super::ConfigEnterStateCoroutine(PMCoroutine);

	// 打开登录
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([this]()
	{
		if (auto* ViewMgr = UMyViewManager::Get(this))
		{
			ViewMgr->CloseAllPage();
		}
		UE_LOG(LogTemp, Log, TEXT("GlobalLoginState: Open Login Page"));
		
		if (auto* ViewMgr = UMyViewManager::Get(this))
		{
			ViewMgr->OpenPage(FUINames::EM_LoginPage, false);
		}
		
		if (const UGlobalLoginStateEventData* TrigData = Cast<const UGlobalLoginStateEventData>(GetTriggerEventData()))
		{
			if (auto PtrGameStartPage = Cast<UGameStartPage>(TrigData->PtrGameStartPageInst))
			{
				PtrGameStartPage->WillClosePage();
				PtrGameStartPage->RemoveFromViewport();
			}
		}
	}));
}

void UGlobalLoginState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}
