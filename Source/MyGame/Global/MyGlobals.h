// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyGameData.h"
#include "MyGlobals.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, config=Engine)
class MYGAME_API UMyGlobals : public UObject
{
	GENERATED_BODY()
public:
	static UMyGlobals& Get();
	static UMyGlobals* SafeGet();
	UMyGameData* GetGameData() const
	{
		return GameData;
	}

protected:
	UPROPERTY()
	UMyGameData* GameData;
};
