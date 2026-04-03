// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MyAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	UMyAssetManager();
	UMyAssetManager(const FObjectInitializer& ObjectInitializer);

	static UMyAssetManager& Get();
	/** Returns the current AssetManager object */

	// 通过StreamingManager加载(TargetToStream如果存在重定向会改路径)
	TSharedPtr<FStreamableHandle> RequestSyncLoad(FSoftObjectPath& TargetToStream, bool bManageActiveHandle = false, FString DebugName = TEXT("RequestSyncLoad Single"));

	void TestStreamableManager();
	void Test();
	TSharedPtr<FStreamableHandle> CurrentLoadHandle;
};


// 游戏必须继承它，在 DefaultGame.ini 指定自定义类，实现项目专属资源管理逻辑。

// UAssetManager 是 UE 全局唯一单例，核心职责：
// 管理PrimaryAsset（主资源）
// 异步 / 同步加载、卸载资源
// 管理 AssetBundle（资源包 / 状态切换）
// 管理 Chunk（分包 / DLC）下载与安装
// 烘焙（Cook）规则、分包规则
// 资源重定向、编辑器资源刷新
// 防止资源被 GC 自动回收
//
// DeterminePrimaryAssetIdForObject：自定义资源 ID
// ScanPrimaryAssetTypesFromConfig：自定义资源扫描
// GetPackageChunkIds：自定义分包 / DLC 规则
// Load/Unload/ChangeBundleState：自定义加载逻辑

//
// 一、核心作用（一句话）
// FStreamableManager 用于异步 / 同步加载任意 UE 资源（蓝图、模型、贴图、音效、DataAsset、关卡等），并通过 FStreamableHandle 控制资源的内存生命周期：加载 → 持有 → 卸载。
// 关键能力
// 异步加载：不卡顿主线程，加载完回调
// 同步加载：立即加载但会卡线程（慎用）
// 内存控制：只要 Handle 不释放 / 不销毁，资源就不会被 GC 卸载
// 批量加载：一次加载多个资源
// 组合加载：等待多个加载任务全部完成
// 软引用安全：专门配合 TSoftObjectPtr / FSoftObjectPath 使用
// 二、业务开发怎么用？（最实用 4 种用法）
// 前提


// 都是通过
// UAssetManager::GetStreamableManager().

// 用法 1：异步加载单个资源（最常用，不卡顿）
// 软引用（你的变量）
// TSoftObjectPtr<UTexture2D> IconPath;
//
// // 异步加载
// TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
// 	IconPath.ToSoftObjectPath(),       // 要加载的资源
// 	FStreamableDelegate::CreateLambda([this, IconPath]() {
// 		// 加载完成回调
// 		UTexture2D* LoadedIcon = IconPath.Get();
// 		if (LoadedIcon)
// 		{
// 			UE_LOG(LogTemp, Log, TEXT("加载成功"));
// 		}
// 	}),
// 	FStreamableManager::DefaultAsyncLoadPriority // 优先级
// );
//
// // 保存 Handle，用来控制生命周期
// CurrentLoadHandle = Handle;