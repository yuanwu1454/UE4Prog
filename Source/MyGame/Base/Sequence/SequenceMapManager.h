// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
#include "SequenceMapManager.generated.h"

// 地图加载状态
UENUM(BlueprintType)
enum class EMapLoadState : uint8
{
	None            UMETA(DisplayName = "无状态"),
	Loading         UMETA(DisplayName = "加载中"),
	Completed       UMETA(DisplayName = "加载完成"),
	Failed          UMETA(DisplayName = "加载失败")
};

// 地图加载事件委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLoadStarted, const FString&, MapName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLoadCompleted, int32, MapId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLoadFailed, int32, MapId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLoadProgress, float, Progress);

/**
 * 重构版：剧情地图管理子系统
 * 职责：地图跳转、异步加载、流关卡管理、生命周期安全控制
 */
UCLASS()
class MYGAME_API USequenceMapManager : public UMyGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 全局获取
	UFUNCTION(BlueprintCallable, Category = "Sequence Map", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static USequenceMapManager* Get(UObject* WorldContextObject);

	// 对外蓝图接口
	UFUNCTION(BlueprintCallable, Category = "Sequence Map")
	bool TeleportMap(int32 MapId);

	UFUNCTION(BlueprintCallable, Category = "Sequence Map")
	bool IsMapLoading() const { return CurrentState == EMapLoadState::Loading; }

	// 地图加载事件（外部业务系统订阅，实现解耦）
	UPROPERTY(BlueprintAssignable, Category = "Sequence Map")
	FOnMapLoadStarted OnMapLoadStarted;

	UPROPERTY(BlueprintAssignable, Category = "Sequence Map")
	FOnMapLoadCompleted OnMapLoadCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Sequence Map")
	FOnMapLoadFailed OnMapLoadFailed;

	UPROPERTY(BlueprintAssignable, Category = "Sequence Map")
	FOnMapLoadProgress OnMapLoadProgress;

protected:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	// 核心流程
	void StartMapLoad(int32 MapId, const FString& TargetMapPath);
	void CancelCurrentLoad();
	void ResetAllData();
	void UpdateLoadState(EMapLoadState NewState);

	// 地图加载流程
	void OpenTransitionLevel();
	void OpenTargetLevelAsync();
	void OnAsyncLevelLoadComplete(const FName& PackageName, UPackage* Package, EAsyncLoadingResult::Type Result);
	void OnTargetLevelLoaded(UWorld* LoadedWorld);

	// 流关卡加载
	bool StartPreloadStreamLevels();
	void LoadNextStreamLevel();
	UFUNCTION()
	void OnStreamLevelLoaded();

	// 资源加载完成（地图+动画）
	void OnAllResourcesLoaded();
	void OnSequenceAssetsLoaded();

	// 全局地图加载完成回调
	UFUNCTION()
	void OnPostLoadMap(UWorld* World);

	// 工具
	bool IsWorldValid() const;
	FString GetShortMapName(const FString& LongMapPath) const;
	bool IsCurrentMap(const FString& MapPath) const;

private:
	// 状态
	EMapLoadState CurrentState = EMapLoadState::None;

	// 目标地图数据
	int32 TargetMapId = INDEX_NONE;
	FString TargetMapPath;
	FString TransitionMapName;

	// 流关卡加载
	TArray<FString> PendingStreamLevels;
	int32 CurrentStreamLevelIndex = INDEX_NONE;

	// 资源加载标记
	bool bStreamLevelsLoaded = false;
	bool bSequenceAssetsLoaded = false;

	// 安全句柄
	FDelegateHandle PostLoadMapHandle;
	FTimerHandle TimeoutHandle;
};