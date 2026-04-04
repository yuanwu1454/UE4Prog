// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MyGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// 必须重写：子系统初始化（相当于 BeginPlay）
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 必须重写：子系统销毁（相当于 EndPlay）
	virtual void Deinitialize() override;

	virtual void OnStartUp(){};
	virtual void OnShutDown(){};

	// 可选重写：控制是否创建这个子系统（比如只在服务器创建）
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

public:
	// ==================== 你自己的业务函数 ====================
	// 示例：蓝图可调用的全局方法
	UFUNCTION(BlueprintCallable, Category = "My Subsystem")
	void TestSubsystemFunction();

	// 示例：获取 GameInstance（引擎自带方法，这里演示用法）
	UFUNCTION(BlueprintCallable, Category = "My Subsystem")
	UGameInstance* GetMyGameInstance() const;
};
