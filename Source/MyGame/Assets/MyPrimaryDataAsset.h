// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyPrimaryDataAsset.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class MYGAME_API UMyPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMyPrimaryDataAsset();
	UMyPrimaryDataAsset(const FObjectInitializer& ObjectInitializer);

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	virtual void PostLoad() override;

#if WITH_EDITORONLY_DATA
	/** This scans the class for AssetBundles metadata on asset properties and initializes the AssetBundleData with InitializeAssetBundlesFromMetadata */
	 virtual void UpdateAssetBundleData() override;

	/** Updates AssetBundleData */
	virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;
#endif
};


// UDataAsset：纯静态数据容器，只用来存配置数据，不参与资源管理，不能主动加载 / 卸载。
// UPrimaryDataAsset：可被资产管理的 DataAsset，继承自 UDataAsset，具备资源生命周期管理能力，是 AssetManager 的核心操作对象。

// UDataAsset = 存数据的小本子，不能单独加载，只能被引用。
// UPrimaryDataAsset = 带身份证 + 行李包的 DataAsset，能被 AssetManager 管理、手动加载卸载、打包关联资源。
// 项目里独立的核心配置一定用 UPrimaryDataAsset，附属子配置用 UDataAsset。

// 四、实战使用场景（怎么选？）
// ✅ 用 UDataAsset 的场景
// 子配置、附属数据：比如一个装备里的属性详情、技能的一段参数；
// 不需要独立加载：它永远被其他资产引用，跟着父资产一起加载；
// 纯数据，无关联资源：只存数值、字符串、简单结构体。
// 例子：角色基础属性表、任务文本配置、小道具参数。
// ✅ 用 UPrimaryDataAsset 的场景
// 独立主配置：装备、技能、武器、地图配置、角色模板；
// 需要手动控制加载 / 卸载（内存优化）；
// 需要批量加载关联资源（贴图、音效、模型）；
// 需要按 ID 查找资产；
// 游戏模块化、热更新必备。
// 例子：可掉落的装备配置、可学习的技能、可切换的角色套装。