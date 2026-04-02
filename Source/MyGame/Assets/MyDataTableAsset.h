// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyDataTableAsset.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyDataTableAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = Data)
	TArray<FSoftObjectPath> StaticLoad;
	
public:
	UMyDataTableAsset();
	void LoadStartupData();
	
	template <class T>
	const T* GetDataTableRow(int32 KeyID) const;

	template <class T>
	const UDataTable* GetDataTable() const;
	const UDataTable* GetDataTable(UScriptStruct* Struct) const;
	
	template <class T>
	const T* GetDataTableRow(int32 KeyID, int32 KeySecondID) const;

	template <class T>
	const T* GetDataTableRow(FName KeyID, int32 KeySecondID) const;

	template <class T>
	const T* GetDataTableRow(const FName& RowName, bool bWarnIfRowMissing) const;
	
	template <class T>
	const T* GetDataTableRow(const FName& RowName) const;

	template <class T>
	bool GetDataTableRows(TArray<T*>& Results) const;

	template <class T>
	int32 ForeachRow(TFunctionRef<void(const FName& Key, const T& Value)> Predicate) const;
	
	FString ContextString = TEXT("UPMDataTableAsset");

	UPROPERTY(Transient, SkipSerialization)
	TMap<const UScriptStruct*, UDataTable*> DataTables;
};

template <class T>
const UDataTable* UMyDataTableAsset::GetDataTable() const
{
	UScriptStruct* RowStruct = T::StaticStruct();
	return GetDataTable(RowStruct);
}


template <class T>
const T* UMyDataTableAsset::GetDataTableRow(int32 KeyID) const
{
	if (const UDataTable* DataTable = GetDataTable<T>())
	{
		return DataTable->FindRow<T>(FName(*FString::FromInt(KeyID)), ContextString);
	}
	return nullptr;
}

template <class T>
const T* UMyDataTableAsset::GetDataTableRow(int32 KeyID, int32 KeySecondID) const
{
	if (const UDataTable* DataTable = GetDataTable<T>())
	{
		return DataTable->FindRow<T>(FName(*FString::Printf(TEXT("%d_%d"), KeyID, KeySecondID)), ContextString);
	}

	return nullptr;
}

template <class T>
const T* UMyDataTableAsset::GetDataTableRow(FName KeyID, int32 KeySecondID) const
{
	if (const UDataTable* DataTable = GetDataTable<T>())
	{
		return DataTable->FindRow<T>(FName(*FString::Printf(TEXT("%s_%d"), *KeyID.ToString(), KeySecondID)), ContextString);
	}

	return nullptr;
}

template <class T>
const T* UMyDataTableAsset::GetDataTableRow(const FName& RowName, bool bWarnIfRowMissing) const
{
	if (const UDataTable* DataTable = GetDataTable<T>())
	{
		return DataTable->FindRow<T>(RowName, ContextString, bWarnIfRowMissing);
	}
	return nullptr;
}

template <class T>
const T* UMyDataTableAsset::GetDataTableRow(const FName& RowName) const
{
	if (const UDataTable* DataTable = GetDataTable<T>())
	{
		return DataTable->FindRow<T>(RowName, ContextString);
	}
	return nullptr;
}

template <class T>
bool UMyDataTableAsset::GetDataTableRows(TArray<T*>& Results) const
{
	Results.Empty();
	if (const UDataTable* DataTable = GetDataTable<T>())
	{
		DataTable->GetAllRows(ContextString, Results);
	}
	return Results.Num() > 0;
}

template <class T>
int32 UMyDataTableAsset::ForeachRow(TFunctionRef<void(const FName& Key, const T& Value)> Predicate) const
{
	if (const UDataTable* DataTable = GetDataTable<T>())
	{
		DataTable->ForeachRow(ContextString, Predicate);
		return DataTable->GetRowMap().Num();
	}

	return 0;
}
