// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
#include "LoadingStream.generated.h"

#define ACTUAL_LERP_SPEED   2
#define SIMULATE_LERP_SPEED 3
#define DELTA_TIME			0.05
#define SIMULATE_LIMIT		98

#define TIP_RANDOM_TIME		5

UENUM(BlueprintType)
enum class ECyLoadingStyle :uint8
{
	None,
	Common,				// 通用
	Login,				// 登录
	Cinematic,			// 剧情
	Map,				// 地图
	Apartment,	        // 休息室
	Force				// 强制关闭loading界面使用
};

UENUM(BlueprintType)
enum class ECyProgressStyle : uint8
{
	None,
	Simulate,            // 模拟
	Actual               // 真实
};


/**
 * 
 */
UCLASS(BlueprintType)
class MYGAME_API ULoadingStream : public UMyGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual	void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION()
	static ULoadingStream* Get(const UObject* ContextObject);
	UFUNCTION()
	void Start(ECyLoadingStyle inLoadingStyle, int32 inId = 0, ECyProgressStyle inProgressStyle = ECyProgressStyle::None);

	UFUNCTION()
	void Percent(float InPercent);

	// 涉及加载地图类的关闭不用传参数，其他加载（通用，登录，剧情，休息室）成对出现。
	UFUNCTION()
	void Stop(ECyLoadingStyle inLoadingStyle = ECyLoadingStyle::Force);

	UFUNCTION()
	bool IsLoading();

	void SetOnlyForceStop(const bool bInOnlyForceStop) { bOnlyForceStop = bInOnlyForceStop; }

private:
	void ProcessData(ECyLoadingStyle inLoadingStyle, int32 inId, ECyProgressStyle inProgressStyle);
	void UpdateLoadingPercent();
	void SetUIPercent(float InPercent);
	void TimerClosePage();
	void UpdateLoadingTip(int32 TipsIndex);
	void ClearData();

private:
	//进度相关变量
	FTimerHandle TimerHandle_UpdateProgress;
	FTimerHandle TimerHandle_ClosePage;
	float CurrentLoadPercent = 0.f;
	float StageLoadPercent = 0.f;
	ECyProgressStyle ProgressStyle = ECyProgressStyle::None;
	float UpdateAlpha = 0.f;

	FTimerHandle TimerHandle_RandomTips;
	TArray<int32> RandomTipsList;

	bool bWorking = false;
	ECyLoadingStyle LoadingStyle = ECyLoadingStyle::None;
	int32 WorkingId = 0;

	//局内声音开关为计数器设计。
	bool bMuteInGameSound = false;

	// 标记当前只能强制停止
	bool bOnlyForceStop = false;
};
