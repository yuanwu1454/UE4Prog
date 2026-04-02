// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDataTableAsset.h"

#include "Engine/StreamableManager.h"
#include "MyAssetManager.h"

UMyDataTableAsset::UMyDataTableAsset()
{
}

void UMyDataTableAsset::LoadStartupData()
{
	for (auto& SoftObjectPath : StaticLoad)
	{
		FSoftObjectPath TmpSOP = SoftObjectPath;
		TSharedPtr<FStreamableHandle> AssetHandle = UMyAssetManager::Get().RequestSyncLoad(TmpSOP);
		if (AssetHandle.IsValid())
		{
			UDataTable* DataTable;
			DataTable = Cast<UDataTable>(TmpSOP.ResolveObject());
			if(DataTable)
			{
				DataTables.Emplace(DataTable->GetRowStruct(), DataTable);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to load PMDataTable: '%s'!"), *TmpSOP.ToString());
			}
		}
	}
}

const UDataTable* UMyDataTableAsset::GetDataTable(UScriptStruct* RowStruct) const
{
	if (RowStruct)
	{
		if (UDataTable* DataTable = DataTables.FindRef(RowStruct))
		{
			return DataTable;
		}
	}
	return nullptr;
}
