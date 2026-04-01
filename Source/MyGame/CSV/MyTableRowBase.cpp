// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTableRowBase.h"

FMyTableRowBase::FMyTableRowBase()
{
}

FMyTableRowBase::~FMyTableRowBase()
{
}

void FMyTableRowBase::OnPostDataImport(const UDataTable* InDataTable, const FName InRowName,
                                       TArray<FString>& OutCollectedImportProblems)
{
	FTableRowBase::OnPostDataImport(InDataTable, InRowName, OutCollectedImportProblems);
}

void FMyTableRowBase::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);
}
