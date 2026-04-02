// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Assets/MyStringTableAsset.h"
#include "Assets/MyDataTableAsset.h"
#include "MySingleTableManagerBase.h"
#include "MyGameData.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyGameData : public UObject
{
	GENERATED_BODY()

	// 不可编辑，意味着可以在蓝图中使用，但是也可以通过持久引用对应的资产
	UPROPERTY()
	UMyStringTableAsset* StringTableAsset;
	UPROPERTY()
	UMyDataTableAsset* DataTableAsset;
	UPROPERTY()
	TMap<UClass*, UMySingleTableManagerBase*> DataTableManagersMap;
public:
	UMySingleTableManagerBase* RegisterTableManager(UClass* Class);
	UMySingleTableManagerBase* GetTableManager(UClass* Class) const;
	const UMyDataTableAsset* GetTableAsset() const
	{
		return DataTableAsset;
	}
	
public:
	void LoadStartupData();
private:
	void LoadStringTable();
	void LoadDataTable();
	void InitDataTableManager();

private:

};
