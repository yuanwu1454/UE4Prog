// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GlobalDelegateManager.generated.h"

#pragma region 启动流程
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartupLuaSystem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterLobbyScene);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalStateTransfer, EGlobalStateType, Param);
#pragma endregion 启动流程



/**
 * 
 */
UCLASS()
class MYGAME_API UGlobalDelegateManager : public UMyGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static UGlobalDelegateManager* Get(UObject* InWorldContextObject)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(InWorldContextObject);
		return UGameInstance::GetSubsystem<UGlobalDelegateManager>(GameInstance);
	}
	
	UPROPERTY(BlueprintCallable)
	FOnStartupLuaSystem OnStartupLuaSystem;
	
	UPROPERTY(BlueprintReadOnly)
	FOnEnterLobbyScene OnEnterLobbyScene;

		
	UPROPERTY(BlueprintCallable)
	FOnGlobalStateTransfer OnGlobalStateTransfer;

};
