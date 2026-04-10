// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingStream.h"

#include "MyLogging.h"
#include "MyViewManager.h"
#include "SubSystem/GlobalDelegateManager.h"

void ULoadingStream::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::Initialize"));
}

void ULoadingStream::Deinitialize()
{
	UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::Initialize"));
	ClearData();
	Super::Deinitialize();
}

ULoadingStream* ULoadingStream::Get(const UObject* ContextObject)
{
	if (ContextObject)
    {
    	UWorld* World = ContextObject->GetWorld();
    	if (World)
    	{
    		return World->GetGameInstance()->GetSubsystem<ULoadingStream>();
    	}
    }
    return nullptr;
}

void ULoadingStream::Start(ECyLoadingStyle inLoadingStyle, int32 inId, ECyProgressStyle inProgressStyle)
{
	UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::Start ID = %d, inLoadingStyle = %d, ECyProgressStyle = %d"), inId, inLoadingStyle, inProgressStyle);


	if (bWorking)
	{
		if (LoadingStyle == inLoadingStyle && WorkingId == inId)
		{
			//重复调用
			return;
		}
		if (ProgressStyle != ECyProgressStyle::None)
		{
			//当前运行为加载进度的先清除
			ClearData();
		}
		// 清空随机tip列表
		if (RandomTipsList.Num() > 0)
		{
			RandomTipsList.Empty();
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RandomTips);
		}
	}
	// 一次启动之后就直接打开页面
	ProcessData(inLoadingStyle, inId, inProgressStyle);
}

void ULoadingStream::Percent(float InPercent)
{
	UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::Progress InPercent = %f"), InPercent);
	StageLoadPercent = FMath::Min(FMath::Max(InPercent, StageLoadPercent),100.0f);
	UpdateAlpha = .0f;
}

void ULoadingStream::Stop(ECyLoadingStyle inLoadingStyle)
{
	if (!bWorking)
	{
		UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::Stop LoadingStream already stop!"));
		return;
	}

	if (inLoadingStyle != ECyLoadingStyle::Force)
	{
		// 只能强制停止
		if (bOnlyForceStop)
		{
			UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::Stop OnlyForceStop Start_Style = %d, Stop_Style = %d "),LoadingStyle,inLoadingStyle);
			return;
		}

		if (LoadingStyle != inLoadingStyle)
		{
			UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::Stop Start_Style != Stop_Style Start_Style = %d, Stop_Style = %d "),LoadingStyle,inLoadingStyle);
			return;
		}
	}

	UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::Stop inloadingStyle = %d"), inLoadingStyle);

	bOnlyForceStop = false;

	if (ProgressStyle != ECyProgressStyle::None)
	{
		StageLoadPercent = 100;
		UpdateAlpha = 1.0f;  //直接关即可
	}
	else
	{
		TimerClosePage();
	}
}

bool ULoadingStream::IsLoading()
{
	return bWorking;
}

void ULoadingStream::ProcessData(ECyLoadingStyle inLoadingStyle, int32 inId, ECyProgressStyle inProgressStyle)
{
	switch (inLoadingStyle)
	{
	case ECyLoadingStyle::Common:
		ProgressStyle = inProgressStyle;
		break;
	case ECyLoadingStyle::Login:
		break;
	default:
		break;
	}
	if (ProgressStyle != ECyProgressStyle::None)
	{
		UpdateAlpha= 0.f;
	}
	bWorking = true;
	LoadingStyle = inLoadingStyle;
	WorkingId = inId;

	if (auto ViewMgr = UMyViewManager::Get(this))
	{
		ViewMgr->OpenPage(FUINames::EM_LoadingAssetPage, false);
	}
	else
	{
		UE_LOG(LogPMLoading, Log, TEXT("UPMViewManager::Get(this) == Null"));
		return;
	}

	if (ProgressStyle != ECyProgressStyle::None)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_UpdateProgress, this, &ULoadingStream::UpdateLoadingPercent, 0.1, true);
	}

// 有randomtip的时候 更新 randomtip 
	if (RandomTipsList.Num() > 0)
	{
		UpdateLoadingTip(0);
	}
}

void ULoadingStream::UpdateLoadingPercent()
{
	if (ProgressStyle == ECyProgressStyle::Simulate)
	{
		UpdateAlpha += DELTA_TIME*SIMULATE_LERP_SPEED;
	}
	else if (ProgressStyle == ECyProgressStyle::Actual)
	{
		UpdateAlpha += DELTA_TIME*ACTUAL_LERP_SPEED;
	}
	UpdateAlpha = FMath::Min(UpdateAlpha, 1.0f);
	// StageLoadPercent 就是最大值
	// 这样就能保证持续增长了 
	// 如果updatealpha 为1  就是到达了终点
	// 如果updatealpha 为0  也是CurrentLoadPercent
	float Percent = FMath::Lerp(CurrentLoadPercent, StageLoadPercent, UpdateAlpha);
	UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::UpdateLoadingPercent Percent = %f"), Percent);
	SetUIPercent(Percent);
}

void ULoadingStream::SetUIPercent(float InPercent)
{
	// 进度条 能够持续上升 所以用max
	CurrentLoadPercent = FMath::Max(InPercent, CurrentLoadPercent);

	//通知ui绘制进度
	UGlobalDelegateManager::Get(this)->OnLoadingAssetDelegate.Broadcast(CurrentLoadPercent);
	
	// 达到尽头后，0.1s 直接关闭界面
	constexpr float EPSILON = 0.0001f;
	if (InPercent >= 100 - EPSILON)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_UpdateProgress);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClosePage, this, &ULoadingStream::TimerClosePage, 0.1f, false);
	}
}

void ULoadingStream::TimerClosePage()
{
	UE_LOG(LogPMLoading, Log, TEXT("UCyLoadingStream::TimerClosePage"));
	if (bMuteInGameSound)
	{
		bMuteInGameSound = false;
		// 加载界面隐藏后恢复局内声音
	}
	if (UGlobalDelegateManager::Get(this))
	{
		UGlobalDelegateManager::Get(this)->OnLoadingAssetPageCloseDelegate.Broadcast(LoadingStyle);	
	}
	ClearData();
	if (auto ViewMgr = UMyViewManager::Get(this))
	{
		ViewMgr->ClosePage(FUINames::EM_LoadingAssetPage);
	}
	else
	{
		UE_LOG(LogPMLoading, Log, TEXT("UViewManager::Get(this) == Null"));
	}
}

void ULoadingStream::UpdateLoadingTip(int32 TipsIndex)
{
	if (RandomTipsList.IsValidIndex(TipsIndex))
	{
		FText TipText;
		if (!TipText.IsEmpty())
		{
			//通知ui绘制提示
			UGlobalDelegateManager::Get(this)->OnLoadingAssetTipDelegate.Broadcast(TipText);
		}
	}
	// 5s 更换一个文字提示
	const int32 NextTipsIndex = (TipsIndex + 1) % RandomTipsList.Num();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RandomTips, FTimerDelegate::CreateUObject(this, &ULoadingStream::UpdateLoadingTip, NextTipsIndex), TIP_RANDOM_TIME, false);

}

void ULoadingStream::ClearData()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_UpdateProgress);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ClosePage);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RandomTips);
	CurrentLoadPercent = .0f;
	StageLoadPercent = .0f;
	ProgressStyle = ECyProgressStyle::None;

	RandomTipsList.Empty();

	bWorking = false;
	LoadingStyle = ECyLoadingStyle::None;
	WorkingId = 0;
	UpdateAlpha = .0f; 
}
