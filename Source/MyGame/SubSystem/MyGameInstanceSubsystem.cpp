// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstanceSubsystem.h"
#include "Engine/GameInstance.h"

void UMyGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 子系统初始化逻辑
	UE_LOG(LogTemp, Log, TEXT("UMyGameInstanceSubsystem 初始化成功！"));

	bIsInitialized = true;
}

void UMyGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();

	// 子系统销毁/清理逻辑
	UE_LOG(LogTemp, Log, TEXT("UMyGameInstanceSubsystem 已销毁！"));

	bIsInitialized = false;
}

bool UMyGameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 父类默认返回 true
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	// 你可以在这里加条件：
	// 例如：只在服务器创建
	// return GetGameInstance()->GetWorld()->GetNetMode() == NM_DedicatedServer;

	return true;
}

void UMyGameInstanceSubsystem::TestSubsystemFunction()
{
	UE_LOG(LogTemp, Log, TEXT("我的自定义子系统方法被调用了！"));
}

UGameInstance* UMyGameInstanceSubsystem::GetMyGameInstance() const
{
	return GetGameInstance();
}

// // 从任何地方获取你的子系统
// if (UGameInstance* GI = GetGameInstance())
// {
// 	if (UMyGameInstanceSubsystem* Subsystem = GI->GetSubsystem<UMyGameInstanceSubsystem>())
// 	{
// 		Subsystem->TestSubsystemFunction();
// 	}
// }

