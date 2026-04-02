// Fill out your copyright notice in the Description page of Project Settings.


#include "MySingleTableManagerBase.h"
#include "Global/MyGlobals.h"
UMySingleTableManagerBase::UMySingleTableManagerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UMySingleTableManagerBase* UMySingleTableManagerBase::Get(UClass* Cls)
{
	UMyGameData* GameData = UMyGlobals::Get().GetGameData();	
	if(const auto SingleTableManager = GameData->GetTableManager(Cls))
	{
		return  SingleTableManager;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load PMDataTableAsset: '%s'!"), *GetNameSafe(Cls));
	}
	return nullptr;
}

void UMySingleTableManagerBase::Init()
{
	if(bIsClientDataInit)
	{
		return;
	}
	bIsClientDataInit = true;
	OnInit();
}
