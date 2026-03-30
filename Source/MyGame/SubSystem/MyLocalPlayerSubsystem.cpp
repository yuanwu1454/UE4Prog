#include "MyLocalPlayerSubsystem.h"
#include "Engine/LocalPlayer.h"

void UMyLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 初始化逻辑
	UE_LOG(LogTemp, Log, TEXT("MyLocalPlayerSubsystem 初始化成功！"));
	bSubsystemInitialized = true;
}

void UMyLocalPlayerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	// 清理逻辑
	UE_LOG(LogTemp, Log, TEXT("MyLocalPlayerSubsystem 已销毁！"));
	bSubsystemInitialized = false;
}

bool UMyLocalPlayerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 父类默认创建
	if (!Super::ShouldCreateSubsystem(Outer))
		return false;

	// 可选：只为主玩家创建
	// ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Outer);
	// return LocalPlayer && LocalPlayer->GetControllerId() == 0;

	return true;
}

void UMyLocalPlayerSubsystem::Use()
{
	// 安全获取（可能返回空）
	auto Player = GetLocalPlayer<ULocalPlayer>();
	if (Player)
	{
		Player->Origin;
		auto SubSystem = Player->GetSubsystem<UMyLocalPlayerSubsystem>();
	}

	// 严格获取（确保不为空，空则崩溃）
	// GetLocalPlayerChecked<ULocalPlayer>();

	// 从 PlayerController 获取
	// APlayerController* PC = GetController<APlayerController>();
	// if (PC && PC->Player)
	// {
	// 	UMyLocalPlayerSubsystem* Subsystem = PC->Player->GetSubsystem<UMyLocalPlayerSubsystem>();
	// 	if (Subsystem)
	// 	{
	// 		Subsystem->TestLocalPlayerSubsystem();
	// 	}
	// }
}

ULocalPlayer* UMyLocalPlayerSubsystem::GetMyLocalPlayer() const
{
	// 直接调用引擎自带的模板方法获取 LocalPlayer
	return GetLocalPlayer<ULocalPlayer>();
}

void UMyLocalPlayerSubsystem::TestLocalPlayerSubsystem()
{
	UE_LOG(LogTemp, Log, TEXT("本地玩家子系统方法被调用！"));
}

// 每个本地玩家独立实例：分屏 / 多本地玩家时，每个玩家都会创建自己的子系统
// 生命周期 = LocalPlayer：玩家加入创建，玩家退出销毁
// 自带获取玩家方法：GetLocalPlayer() 引擎已实现，直接用
// 所属 Outer = LocalPlayer：通过 GetOuter() 直接拿到玩家