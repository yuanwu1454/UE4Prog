// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStartPage.h"

#include "MyGameInstance.h"
#include "MediaAssets/Public/FileMediaSource.h"
#include "MediaAssets/Public/MediaPlayer.h"
#include "MediaAssets/Public/MediaPlaylist.h"

/**
 * @brief 控件初始化完成时调用，配置视频播放列表与超时计时器
 */
void UGameStartPage::NativeOnInitialized()
{
	// 获取游戏实例（可用于全局数据/逻辑）
	auto GI = GetWorld()->GetGameInstance<UMyGameInstance>();

	// 配置Logo视频播放列表
	if (LogoVideoPlaylist)
	{
		// 清空播放列表第0个元素（清理默认/旧视频）
		LogoVideoPlaylist->RemoveAt(0);

		// 将指定视频文件添加到播放列表
		if (LogoFile)
		{
			LogoVideoPlaylist->AddFile(LogoFile->FilePath);
		}

		// 媒体播放器绑定播放列表，设置不循环播放
		if (VideoMediaPlayer)
		{
			VideoMediaPlayer->OpenPlaylist(LogoVideoPlaylist);
			VideoMediaPlayer->SetLooping(false);
		}
	}

	// 获取世界计时器管理器
	auto& TimerManager = GetWorld()->GetTimerManager();

	// 绑定视频播放完成回调
	if (VideoMediaPlayer)
	{
		// 视频播放完毕后，自动执行AfterPlayTXLogo逻辑
		VideoMediaPlayer->OnEndReached.AddDynamic(this, &UGameStartPage::AfterPlayTXLogo);
	}

	// 设置4秒超时保护：无论视频是否播完，4秒后强制进入下一步
	TimerManager.SetTimer(OutTimeTimerHandle, FTimerDelegate::CreateUObject(this, &UGameStartPage::AfterPlayTXLogo), 4, false);
}

/**
 * @brief 清理超时计时器，安全停止并重置句柄状态
 */
void UGameStartPage::ClearOutTimeTimer()
{
	// 判断计时器句柄是否有效
	if (OutTimeTimerHandle.IsValid())
	{
		// 安全获取世界，清除正在运行的计时器
		if (auto World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(OutTimeTimerHandle);
		}

		// 将计时器句柄标记为无效，确保IsValid()返回false，避免重复清理
		OutTimeTimerHandle.Invalidate();
	}
}

/**
 * @brief 开始播放Logo视频
 */
void UGameStartPage::StartPresentation()
{
	if (VideoMediaPlayer)
	{
		VideoMediaPlayer->Play();
	}
}

/**
 * @brief Logo视频播放完成/超时后执行的收尾逻辑
 */
void UGameStartPage::AfterPlayTXLogo()
{
	// 立即清理超时计时器，防止重复触发
	ClearOutTimeTimer();

	// 关闭媒体播放器，解绑所有委托，释放资源
	if (VideoMediaPlayer)
	{
		VideoMediaPlayer->OnMediaOpened.Clear();
        VideoMediaPlayer->OnEndReached.Clear();
        VideoMediaPlayer->Close();
	}

	// 播放下一个Logo淡入动画
	CyLogoFadeIn();
}

/**
 * @brief 播放公司Logo淡入动画
 */
void UGameStartPage::CyLogoFadeIn()
{
	// 获取淡入动画资源
	UWidgetAnimation* PlatformAnim = AnimLogoFadeIn;

	if (PlatformAnim)
	{
		// 绑定动画结束回调
		FWidgetAnimationDynamicEvent AnimEndDelegate;
		AnimEndDelegate.BindDynamic(this, &UGameStartPage::StartFlowEnd);
		BindToAnimationFinished(PlatformAnim, AnimEndDelegate);

		// 执行淡入动画
		PlayAnimation(PlatformAnim);
	}
	else
	{
		// 无动画时直接进入结束流程
		StartFlowEnd();
	}
}

/**
 * @brief 开场动画全部完成，触发外部绑定的结束事件
 */
void UGameStartPage::StartFlowEnd()
{
	AnimEndEvent.ExecuteIfBound();
}

/**
 * @brief 页面即将关闭时调用，做资源清理
 */
void UGameStartPage::WillClosePage()
{
	// 关闭页面时必须清理计时器，防止野指针/逻辑错误
	ClearOutTimeTimer();
}