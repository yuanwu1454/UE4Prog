// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
#include "GASInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UGASInstanceSubsystem : public UMyGameInstanceSubsystem
{
public:
	// 全局获取
	UFUNCTION(BlueprintCallable, Category = "Sequence Map", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UGASInstanceSubsystem* Get(UObject* WorldContextObject);

	GENERATED_BODY()
	void ShowASCTag(class UAbilitySystemComponent* ASC);
};
