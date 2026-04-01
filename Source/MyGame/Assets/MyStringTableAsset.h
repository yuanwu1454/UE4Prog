// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyStringTableAsset.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyStringTableAsset : public UDataAsset
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<class UStringTable*>					StringTables;
};
