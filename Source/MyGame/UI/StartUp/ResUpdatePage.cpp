// Fill out your copyright notice in the Description page of Project Settings.

#include "ResUpdatePage.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

/**
 * @brief 控件初始化时调用
 * 重置进度条为0，确保界面初始状态正确
 */
void UResUpdatePage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 初始化进度条，设置进度为0
	if (ProgressBar)
	{
		ProgressBar->SetPercent(0.0f);
	}
}

/**
 * @brief 设置目标进度（平滑过渡用，不会瞬间跳变）
 * @param ProgressValue 目标进度 0~1
 */
void UResUpdatePage::SetProgress(float ProgressValue)
{
	// 只有传入的新进度大于当前进度时才更新，防止回退
	if (ProgressBar && ProgressBar->Percent <= ProgressValue)
	{
		// 记录目标进度
		TargetProgressValue = ProgressValue;

		// 根据总过渡时间计算每帧需要增加的速度（平滑追赶算法）
		CatchUpSpeed = (TargetProgressValue - ProgressBar->Percent) / CatchUpTime;
	}
	else
	{
		// 日志：新进度比当前进度小，忽略本次设置（避免进度回退）
		UE_LOG(LogTemp, Log, TEXT("Set delay progress value %f < %f"), ProgressValue, ProgressBar ? ProgressBar->Percent : 0.0f);
	}
}

/**
 * @brief 获取当前进度条真实进度
 * @return 0~1之间的进度值
 */
float UResUpdatePage::GetProgress()
{
	if(ProgressBar)
	{
		return ProgressBar->Percent;
	}
	return 0.0f;
}

/**
 * @brief 设置下载进度文本（如：50% / 100MB）
 * @param ProgText 要显示的字符串
 */
void UResUpdatePage::SetDownladProgressText(FString ProgText)
{
	if (Txt_Progress)
	{
		Txt_Progress->SetText(FText::FromString(ProgText));
	}
}

/**
 * @brief 设置当前更新阶段文本（如：检查更新、下载中、解压中）
 * @param Str 阶段描述
 */
void UResUpdatePage::SetPhaseString(FString Str)
{
	if (Txt_Phase)
	{
		Txt_Phase->SetText(FText::FromString(Str));
	}
}

/**
 * @brief 每帧调用，实现进度条平滑滚动效果
 */
void UResUpdatePage::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ProgressBar)
	{
		// 计算当前帧应该设置的进度：按速度追赶，不超过目标值
		float PendingSetProgress = FMath::Min(TargetProgressValue, ProgressBar->Percent + CatchUpSpeed * InDeltaTime);

		// 把进度限制在 0~1 范围内并设置到进度条
		ProgressBar->SetPercent(FMath::Clamp(PendingSetProgress, 0.f, 1.f));
	}
}