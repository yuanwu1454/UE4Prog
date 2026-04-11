// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Types.h"
#include "GlobalDelegateManager.generated.h"

#pragma region 启动流程
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartupLuaSystem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterLobbyScene);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalStateTransfer, EGlobalStateType, Param);
#pragma endregion 启动流程

#pragma region 通用动态代理事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynDelegateEmptyParam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynDelegateTwoParam, int, IntParam, FString, StringParam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynDelegateOneStringParam, FString, Param);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynDelegateOneFloatParam, float, Param);
#pragma endregion 通用动态代理事件 


#pragma region 加载进度
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePlayLoadingStateChange, const FGamePlayLoadingStateParams&, LoadingParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostLoadMapWithWorld, const UWorld*, World);
#pragma endregion 加载进度


#pragma region 单局
// 重尝试重连回DS
DECLARE_DYNAMIC_DELEGATE(FTryReconnectToDS);
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandleNetworkFailure, const FGameTransitionReason&, Reason);
// // 单局表现阶段
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayingShowStageChange, EGamePlayingStage, Stage);
// // 局内GameState Begin Play事件
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayingGameStateBeginPlay, APMGameState*, GS);
// // ds踢出玩家
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDSKickOutPlayer, const ECyDSKickOutPlayerReasonType, InExitGameReason);
// 触发结算系统
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerResultSystem);
// 触发结算个人展示UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResultPersonalShowPage);
// Sequence触发展示合影界面UI
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResultSequenceShowGroupPhotoView);
#pragma endregion 单局


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingAssetDelegate, float, percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingAssetTipDelegate, FText, tip);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingAssetPageCloseDelegate, ECyLoadingStyle, LoadingStyle);

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

	#pragma region 启动流程
	UPROPERTY(BlueprintCallable)
	FOnStartupLuaSystem OnStartupLuaSystem;
	UPROPERTY(BlueprintReadOnly)
	FOnEnterLobbyScene OnEnterLobbyScene;
	UPROPERTY(BlueprintCallable)
	FOnGlobalStateTransfer OnGlobalStateTransfer;
    UPROPERTY(BlueprintCallable)
    FDynDelegateTwoParam OpenWillReconnectToMap;
	#pragma endregion 启动流程

	#pragma region Loading
    // 加载Loading的控制
    UPROPERTY(BlueprintCallable)
    FDynDelegateEmptyParam OpenLoadingMap;
    UPROPERTY(BlueprintCallable)
    FDynDelegateEmptyParam EndLoadingMap;
    UPROPERTY(BlueprintCallable)
    FOnGamePlayLoadingStateChange OnGamePlayLoadingStateChange;
    // 在等待大厅休息室加载完毕
    UPROPERTY(BlueprintCallable)
    FDynDelegateEmptyParam OnWaitingLobbyLoadingFinish;
    #pragma endregion Loading

	//资源加载进度
	UPROPERTY(BlueprintCallable)
	FOnLoadingAssetDelegate OnLoadingAssetDelegate;

	//资源加载提示
	UPROPERTY(BlueprintCallable)
	FOnLoadingAssetTipDelegate OnLoadingAssetTipDelegate;
	
	//loading界面关闭通知
	UPROPERTY(BlueprintCallable)
	FOnLoadingAssetPageCloseDelegate OnLoadingAssetPageCloseDelegate;


#pragma region Game
	UPROPERTY(BlueprintReadOnly)
	FDynDelegateEmptyParam OnBeginGameDelegateSubsystem;
	UPROPERTY(BlueprintReadOnly)
	FDynDelegateEmptyParam OnEndGameDelegateSubsystem;

	// // 局内进入选角及结算表演状态 
	// UPROPERTY(BlueprintReadOnly)
	// FOnPlayingShowStageChange OnPlayingShowStageChange;
	//
	// UPROPERTY(BlueprintReadOnly)
	// FOnPlayingGameStateBeginPlay OnPlayingGameStateBeginPlay;

	UPROPERTY(BlueprintReadOnly)
	FOnTriggerResultSystem OnTriggerResultSystem;

	UPROPERTY(BlueprintReadOnly)
	FOnResultPersonalShowPage OnResultPersonalShowPage;

	UPROPERTY(BlueprintReadOnly)
	FOnResultSequenceShowGroupPhotoView OnResultSequenceShowGroupPhotoView;
#pragma endregion Game 
};
