// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGlobals.h"
#include "MySingleTableManagerBase.h"
#include "UObject/Object.h"
#include "MyBasicTableManager.generated.h"

/**
 * 
 */
struct FMyUITableRow;
UCLASS()
class MYGAME_API UMyBasicTableManager : public UMySingleTableManagerBase
{
	GENERATED_BODY()
public:
	const static UMyBasicTableManager* Get();

	const FMyUITableRow* GetUITable(const FName& UIName) const;
	
	template <class T>
	static const FMyUITableRow* GetUITable(const FName& UIName);

	const FMyUITableRow* GetUITableWithClass(UClass* InClass) const;
};
template <class T>
const FMyUITableRow* UMyBasicTableManager::GetUITable(const FName& UIName)
{
	const T* RowData = UMyGlobals::Get().GetTableAsset()->GetDataTableRow<T>(UIName);
	return reinterpret_cast<const FMyUITableRow*>(RowData);
}
