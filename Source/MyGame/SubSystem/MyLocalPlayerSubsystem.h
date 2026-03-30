// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "MyLocalPlayerSubsystem.generated.h"

/**
 * 本地玩家子系统
 * 生命周期 = 本地玩家（LocalPlayer）的生命周期
 * 每个本地玩家都会独立创建一个实例
 */
UCLASS()
class MYGAME_API UMyLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	// 引擎子系统核心生命周期
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	void Use();
public:
	// ==================== 自定义功能 ====================
	// 蓝图调用：获取当前所属的本地玩家
	UFUNCTION(BlueprintCallable, Category = "Local Player Subsystem")
	ULocalPlayer* GetMyLocalPlayer() const;

	// 蓝图调用：测试方法
	UFUNCTION(BlueprintCallable, Category = "Local Player Subsystem")
	void TestLocalPlayerSubsystem();

private:
	// 自定义成员变量
	bool bSubsystemInitialized = false;
};