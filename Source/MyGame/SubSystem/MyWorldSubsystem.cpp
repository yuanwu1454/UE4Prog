// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWorldSubsystem.h"
#include "MyWorldSubsystem.h"
#include "Engine/World.h"

void UMyWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("MyWorldSubsystem 初始化成功！"));
}

void UMyWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("MyWorldSubsystem 已销毁！"));
}

bool UMyWorldSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 先执行父类判断
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	// 额外自定义条件（可选）
	return true;
}

void UMyWorldSubsystem::Use()
{
	// 从任何 Actor / 组件里获取
	UWorld* World = GetWorld();
	if (World)
	{
		UMyWorldSubsystem* Subsystem = World->GetSubsystem<UMyWorldSubsystem>();
		if (Subsystem)
		{
			Subsystem->TestWorldSubsystemFunction();
		}
	}
}

bool UMyWorldSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return Super::DoesSupportWorldType(WorldType);
}

void UMyWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickTimer += DeltaTime;
	if (TickTimer >= 1.0f)
	{
		TickTimer = 0.0f;
		// UE_LOG(LogTemp, Log, TEXT("世界子系统每1秒执行一次！"));
	}
}

bool UMyWorldSubsystem::IsTickable() const
{
	return Super::IsTickable();
}

bool UMyWorldSubsystem::IsTickableWhenPaused() const
{
	return Super::IsTickableWhenPaused();
}

void UMyWorldSubsystem::TestWorldSubsystemFunction()
{
	UE_LOG(LogTemp, Log, TEXT("调用了自定义世界子系统方法！"));
}

// ✅ 关键：必须这样写
TStatId UMyWorldSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyWorldSubsystem, STATGROUP_Tickables);
}

// UWorldSubsystem = 当前地图 / 世界的全局子系统
// 切换关卡 → 旧世界销毁 → 子系统自动销毁
// 新世界加载 → 子系统自动重建
// 适合：当前关卡的全局管理、场景逻辑、全局 Actor 管理