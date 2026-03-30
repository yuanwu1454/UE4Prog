#include "MyDynamicEngineSubsystem.h"

void UMyDynamicEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("【动态子系统】已初始化！"));
}

void UMyDynamicEngineSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Log, TEXT("【动态子系统】已销毁！"));
}

void UMyDynamicEngineSubsystem::TestDynamicSubsystem()
{
	UE_LOG(LogTemp, Log, TEXT("动态子系统方法调用成功！"));
}