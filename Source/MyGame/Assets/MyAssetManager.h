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