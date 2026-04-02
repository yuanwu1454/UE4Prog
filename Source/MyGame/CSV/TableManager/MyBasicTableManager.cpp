// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBasicTableManager.h"

#include "MyUITableRow.h"

const UMyBasicTableManager* UMyBasicTableManager::Get()
{
	const auto Manager = UMySingleTableManagerBase::Get(StaticClass());
	return Cast<UMyBasicTableManager>(Manager);
}

const FMyUITableRow* UMyBasicTableManager::GetUITable(const FName& UIName) const
{
	return UMyGlobals::Get().GetTableAsset()->GetDataTableRow<FMyUITableRow>(UIName);
}

const FMyUITableRow* UMyBasicTableManager::GetUITableWithClass(UClass* InClass) const
{
	TArray<FMyUITableRow*> UITableRows;
	if (UMyGlobals::Get().GetTableAsset()->GetDataTableRows(UITableRows))
	{
		for (auto& TableRow : UITableRows)
		{
			if (TableRow->PageClass == InClass)
			{
				return TableRow;
			}
		}
	}
	return nullptr;
}
