// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameData.h"

#include "MyGlobals.h"
#include "MySingleTableManagerBase.h"

UMySingleTableManagerBase* UMyGameData::RegisterTableManager(UClass* Class)
{
	UMySingleTableManagerBase* Manager = NewObject<UMySingleTableManagerBase>(this, Class);
	DataTableManagersMap.Add(Class, Manager);
	Manager->Init();
	return Manager;
}

UMySingleTableManagerBase* UMyGameData::GetTableManager(UClass* Class) const
{
	if(DataTableManagersMap.Contains(Class))
	{
		return DataTableManagersMap.FindRef(Class);
	}
	return nullptr;
}

void UMyGameData::LoadStartupData()
{
	LoadStringTable();
	LoadDataTable();
	InitDataTableManager();
}

void UMyGameData::LoadStringTable()
{
	if(!UMyGlobals::Get().StringTableAsset.IsNull())
	{
		StringTableAsset = Cast<UMyStringTableAsset>(UMyGlobals::Get().StringTableAsset.TryLoad());
	}
}

void UMyGameData::LoadDataTable()
{
	const FSoftObjectPath* DataTableAssetRefToUse = &UMyGlobals::Get().DataTableAssetRef;
	if (DataTableAssetRefToUse->ToString().Len() > 0)
	{
		DataTableAsset = LoadObject<UMyDataTableAsset>(NULL, *DataTableAssetRefToUse->ToString(), NULL, LOAD_None, NULL);
	}
	
	if (!DataTableAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load PMDataTableAsset: '%s', game will use inferior default settings!"), *DataTableAssetRefToUse->ToString());
		// None in ini, so build a default
		DataTableAsset = NewObject<UMyDataTableAsset>(UMyDataTableAsset::StaticClass());
	}
	check(DataTableAsset);
	DataTableAsset->LoadStartupData();
}

void UMyGameData::InitDataTableManager()
{
	TArray<UClass*> DataTableManagerClasses;
	GetDerivedClasses(UMySingleTableManagerBase::StaticClass(), DataTableManagerClasses, true);

	for (UClass* DataTableClass : DataTableManagerClasses)
	{
		RegisterTableManager(DataTableClass);
	}
}
