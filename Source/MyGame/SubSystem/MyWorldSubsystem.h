// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MyWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	// 引擎子系统生命周期函数
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	void Use();
protected:
	// 控制：这个子系统支持哪些类型的世界
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

private:
	// ==================== FTickableGameObject 必须实现 ====================
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableWhenPaused() const override;

public:
	// ==================== 你的自定义功能 ====================
	UFUNCTION(BlueprintCallable, Category = "World Subsystem")
	void TestWorldSubsystemFunction();

private:
	// 示例：每帧更新的计时器
	float TickTimer = 0.0f;
};
