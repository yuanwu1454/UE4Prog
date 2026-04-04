// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "MyGlobalDelegateManager.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyGlobalDelegateManager : public UMyGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static UMyGlobalDelegateManager* Get(UObject* InWorldContextObject)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(InWorldContextObject);
		return UGameInstance::GetSubsystem<UMyGlobalDelegateManager>(GameInstance);
	}
};
