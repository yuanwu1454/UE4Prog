// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Assets/MyStringTableAsset.h"
#include "MyGameData.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyGameData : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY()
	UMyStringTableAsset* StringTableAsset;
};
