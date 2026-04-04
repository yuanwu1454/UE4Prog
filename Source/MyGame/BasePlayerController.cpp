// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "Engine/NetConnection.h"
#include "Kismet/GameplayStatics.h"
#include "Log/MultiplayerLogHelper.h"
#include "MyCheatManager.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Player.h"
#include "Slate/SceneViewport.h"

ABasePlayerController::ABasePlayerController()
{
	// 构造时创建所有 Router（轻量级对象，无性能损耗）

	RoomRouter = nullptr;
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 初始化所有 Router（绑定 PlayerController）
	RoomRouter = NewObject<URoomRouter>(this);
	RoomRouter->Init(this);

	ForceCaptureMouse();
	
	FSlateApplication& SlateApp = FSlateApplication::Get();
	// 焦点
	AppActivatedHandle = SlateApp.OnApplicationActivationStateChanged().AddUObject(this, &ABasePlayerController::OnApplicationActivationStateChanged);
}

void ABasePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FSlateApplication& SlateApp = FSlateApplication::Get();
	SlateApp.OnApplicationActivationStateChanged().Remove(AppActivatedHandle);
	
	Super::EndPlay(EndPlayReason);
}

void ABasePlayerController::TogglePrimitiveComponentRendering(bool bEnableRender)
{
	// 1. 更新开关状态
	bRenderPrimitiveComponents = bEnableRender;

	// 2. 如果重新开启渲染，强制刷新隐藏组件列表（避免渲染异常）
	if (bEnableRender)
	{
		// 手动触发隐藏列表重建（需传入相机位置，这里取当前相机位置）
		FVector ViewLocation;
		FRotator ViewRotation;
		GetPlayerViewPoint(ViewLocation, ViewRotation);
        
		TSet<FPrimitiveComponentId> HiddenComponents;
		BuildHiddenComponentList(ViewLocation, HiddenComponents);
        
		// // 通知渲染层更新（UE4.27内置方法）
		// GetWorld()->GetRenderer()->NotifyRenderStateChanged();
	}
}

//参数ViewLocation：当前相机的世界坐标，是判断 “是否隐藏 Actor” 的核心参考（比如相机穿透 Actor、距离过近 / 过远等）；
// 目的：
// 有的项目需要 “相机穿透墙壁时隐藏墙壁”；
// 有的项目需要 “玩家靠近 NPC 时隐藏 NPC 的武器”；
// 有的项目需要 “根据剧情阶段隐藏特定场景物体”；
void ABasePlayerController::UpdateHiddenActors(const FVector& ViewLocation)
{
	// 1. 先清空旧的HiddenActors列表（避免残留无效Actor）
	HiddenActors.Empty();

	// 2. 遍历世界中所有带"HideWhenPenetrated"标签的Actor（可自定义标签）
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsWithTag(World, FName("HideWhenPenetrated"), AllActors);

	// 3. 逐个判断：相机是否穿透该Actor，若是则加入HiddenActors
	for (AActor* Actor : AllActors)
	{
		if (!Actor || Actor->IsHidden())continue; // 跳过空指针/已隐藏的Actor

		if (IsCameraPenetratingActor(Actor, ViewLocation))
		{
			// 加入HiddenActors列表（后续BuildHiddenComponentList会处理）
			HiddenActors.Add(Actor);
			// 同步标记Actor为隐藏（核心：修改组件渲染状态）
			Actor->SetActorHiddenInGame(true);

			CheatClass = UMyCheatManager::StaticClass();
		}
	}
}
		
void ABasePlayerController::GetLocalPlayerViewport()
{
	// 1. 判断Player是否有效，且是本地玩家（ULocalPlayer）
	if (Player && Player->IsA<ULocalPlayer>())
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

		// 2. 获取该玩家绑定的视口（UGameViewportClient）
		UGameViewportClient* Viewport = LocalPlayer->ViewportClient;
		if (Viewport)
		{
			// 用途：设置视口分辨率、获取鼠标位置、绑定UI等
			FVector2D ViewportSize;
			Viewport->GetViewportSize(ViewportSize);
			UE_LOG(LogTemp, Log, TEXT("本地玩家视口分辨率：%fx%f"), ViewportSize.X, ViewportSize.Y);

		}
	}
}
// 辅助方法：射线检测判断相机是否在Actor碰撞体内
bool ABasePlayerController::IsCameraPenetratingActor(AActor* Actor, const FVector& CameraLocation)
{
	if (!Actor || !Actor->GetRootComponent()) return false;

	// 构建碰撞检测参数
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this->GetPawn()); // 忽略玩家自身
	Params.bTraceComplex = false; // 简单碰撞检测，提升性能

	// 检测：从相机位置向任意方向发射短射线，判断是否命中目标Actor
	FHitResult HitResult;
	FVector TraceEnd = CameraLocation + FVector(1.0f, 0, 0); // 短射线（1cm）
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		TraceEnd,
		ECC_WorldDynamic, // 碰撞通道（可根据项目自定义）
		Params
	);

	// 如果命中的Actor是目标Actor，说明相机在其内部（穿透）
	return (bHit && HitResult.GetActor() == Actor);
}

void ABasePlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& HiddenComponents)
{
    // 第一步：先执行引擎默认逻辑（保留HiddenActors/HiddenPrimitiveComponents的处理结果）
    Super::UpdateHiddenComponents(ViewLocation, HiddenComponents);

    // 第二步：自定义逻辑——隐藏带"HideComponent"标签的组件
    TArray<UPrimitiveComponent*> TargetComponents;
    GetComponentsWithTag(FName("HideComponent"), TargetComponents);

    for (UPrimitiveComponent* Comp : TargetComponents)
    {
        if (!Comp || !Comp->IsRegistered()) continue; // 跳过无效/未注册组件

        // 条件1：相机距离组件小于500cm时隐藏
        float Distance = FVector::Distance(ViewLocation, Comp->GetComponentLocation());
        if (Distance < 500.0f)
        {
            // 直接添加组件ID到渲染层隐藏列表
            HiddenComponents.Add(Comp->ComponentId);
            // 同步标记组件为隐藏（确保状态一致）
            Comp->SetHiddenInGame(true);
        }
        else
        {
            // 距离超出阈值，从隐藏列表移除（恢复显示）
            HiddenComponents.Remove(Comp->ComponentId);
            Comp->SetHiddenInGame(false);
        }
    }
}

// 辅助方法：遍历世界，获取带指定标签的所有PrimitiveComponent
void ABasePlayerController::GetComponentsWithTag(FName Tag, TArray<UPrimitiveComponent*>& OutComponents)
{
    OutComponents.Empty();
    UWorld* World = GetWorld();
    if (!World) return;

    // 遍历所有Actor，提取带标签的组件
    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        if (!Actor) continue;

        TInlineComponentArray<UPrimitiveComponent*> Components;
        Actor->GetComponents(Components);
        for (UPrimitiveComponent* Comp : Components)
        {
            if (Comp && Comp->ComponentHasTag(Tag))
            {
                OutComponents.Add(Comp);
            }
        }
    }
}

void ABasePlayerController::SetDynamicClientCap(int32 NewCap)
{
	if (!HasAuthority()) return; // 仅服务端可修改

	// 确保值≥2600（否则不生效）
	ClientCap = FMath::Max(NewCap, 2600);
	// GetNetConnection()->SetClientBandwidthLimit(ClientCap);
}

// 当已有地图切换（Map Change）操作在进行中时，延迟等待并在合适时机重新准备新的地图切换。
// 使用场景
// 玩家快速重复触发地图切换（如误操作多次点击加载关卡按钮）；
// 联网场景下，服务器 / 客户端地图切换不同步，需要延迟对齐；
// 地图切换前需要等待资源卸载、网络确认等前置操作完成。
void ABasePlayerController::DelayedPrepareMapChange()
{
	Super::DelayedPrepareMapChange();
}

// GetSeamlessTravelActorList 是 UE4/UE5 中 PlayerController 类的虚函数，专用于无缝关卡切换（Seamless Travel） 场景
// —— 在客户端侧，它的核心作用是指定哪些 Actor 需要在关卡切换时被保留并迁移到新关卡中，
// 服务器侧则在 gamemode那边处理
// 无缝关卡切换（Seamless Travel）：区别于普通的地图加载（会黑屏 / 加载界面），无缝切换是 “无感知” 的关卡跳转，常用于开放世界的大地图分区、副本入口等场景；
// 默认迁移规则（无需手动添加）：
// PlayerController 自身；
// 角色权限 < ROLE_Authority 的本地 Actor（如客户端本地特效）；
// 外层（Outer）是迁移名单中 Actor 的非 Actor 对象（如 Actor 内的组件、变量）；
// 仅支持迁移的 Actor 类型：只有 PersistentLevel 中的动态 Actor（游戏过程中生成的 Actor，而非关卡编辑器中放置的静态 Actor）能被迁移。
void ABasePlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
}

// 无缝切换关卡时，如果引擎为玩家创建了新的 PlayerController（而非复用旧的），就会触发这个函数，
// 让你有机会 “收尾” 旧 PC 的资源（比如本地后处理效果、UI 缓存、自定义定时器等），保证新旧 PC 的状态隔离。
// 触发条件
// 仅在无缝关卡切换时触发（普通地图加载不会触发）；
// 仅当 “切换前的旧 PC” 和 “切换后的新 PC” 不是同一个对象时触发（复用旧 PC 则不调用）；
// 客户端和服务器侧的 PlayerController 都会触发（需区分权限处理）。
void ABasePlayerController::SeamlessTravelTo(APlayerController* NewPC)
{
	Super::SeamlessTravelTo(NewPC);
}

// 核心作用是从旧 PC 中拷贝需要持久化的核心数据到新 PC，保证玩家的自定义设置、状态在无缝切换后不丢失。
// 仅在无缝关卡切换时触发（普通地图加载不触发）；
// 仅当新旧 PC 不是同一个对象时触发（复用旧 PC 则不调用）；
// 触发在新 PC 初始化完成后、旧 PC 被清理前，此时新旧 PC 都处于有效状态；
// 客户端和服务器侧的 PC 都会触发（需区分权限处理）。
void ABasePlayerController::SeamlessTravelFrom(APlayerController* OldPC)
{
	Super::SeamlessTravelFrom(OldPC);
}

// 简单来说，它是无缝切换的 “收尾钩子”：
// 专注于切换后的 “最终校准”，比如重新绑定输入、恢复 UI 状态、校准相机、同步服务器数据等。
void ABasePlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
}

// 2. PlayerController 会在构建输入栈时，自动加入 Pawn 的 InputComponent
// 真正的输入栈不是 CurrentInputStack 那一个数组，而是 PlayerController::BuildInputStack() 动态构建的完整栈。
void ABasePlayerController::BuildInputStack(TArray<UInputComponent*>& InputStack)
{
	Super::BuildInputStack(InputStack);
}

void ABasePlayerController::SendToConsole(const FString& Command)
{
	Super::SendToConsole(Command);
	MULTI_LOG(FString::Printf(TEXT("SendToConsole %s"), *Command), this, this);
}

void ABasePlayerController::NotifyLoadedWorld(FName WorldPackageName, bool bFinalDest)
{
	Super::NotifyLoadedWorld(WorldPackageName, bFinalDest);

	// 第二步：自定义逻辑（根据业务需求扩展）
	// 1. 打印日志，确认客户端加载完成（调试用）

	MULTI_LOG(FString::Format(TEXT("客户端已加载新地图：%s，是否最终地图：%s"), 
		{*WorldPackageName.ToString(), bFinalDest ? TEXT("是") : TEXT("否")}), this, this);

	// 2. 如果是最终目标地图，执行初始化逻辑
	if (bFinalDest)
	{
		// 示例1：同步玩家数据（比如血量、道具）

		// 示例2：激活玩家控制权（无缝旅行后可能暂时禁用，需手动开启）

		// 示例3：播放新地图的欢迎剧情/音效
	}
}

void ABasePlayerController::ForceCaptureMouse()
{
	if (IsLocalPlayerController())
	{

		// 1. 判断Player是否有效，且是本地玩家（ULocalPlayer）
		if (Player && Player->IsA<ULocalPlayer>())
		{
			ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

			// 2. 获取该玩家绑定的视口（UGameViewportClient）
			UGameViewportClient* Viewport = LocalPlayer->ViewportClient;
			if (Viewport)
			{
				Viewport->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
				Viewport->SetMouseLockMode(EMouseLockMode::LockInFullscreen);
			}
		}
	}
}

void ABasePlayerController::SwitchCursor(EMouseCursor::Type CursorType, TSubclassOf<UUserWidget> CursorWidgetClass)
{
	if (CursorWidgetClass)
	{
		// 创建自定义光标Widget实例
		CustomCursorWidget = CreateWidget<UUserWidget>(this, CursorWidgetClass);
		if (CustomCursorWidget)
		{
			// 设置Widget为指定光标类型的样式
			SetMouseCursorWidget(CursorType, CustomCursorWidget);
			UE_LOG(LogTemp, Warning, TEXT("已替换光标类型 %d 为自定义Widget"), (int)CursorType);
		}
	}
}

void ABasePlayerController::BeginPlayingState()
{
	// 玩家拥有 Pawn 后 一般来讲就是对拥有的pawn做处理
	Super::BeginPlayingState();
	// 记录新客户端登录，传入PlayerController作为PlayerContext
	MULTI_LOG(FString::Printf(TEXT("BeginPlayingState")), this, this);
}

void ABasePlayerController::EndPlayingState()
{
	MULTI_LOG(FString::Printf(TEXT("EndPlayingState")), this, this);
	Super::EndPlayingState();
}

bool ABasePlayerController::IsLocalPlayerController()
{
	// 核心逻辑：通过Player判断是否为本地玩家
	return Player != nullptr && Player->IsA<ULocalPlayer>();
}

bool ABasePlayerController::IsNetPlayerController()
{
	// 判断是否为联网玩家（服务器端）
	return Player != nullptr && Player->IsA<UNetConnection>();
}

void ABasePlayerController::OnApplicationActivationStateChanged(bool bIsActive){
	if (bIsActive)
	{
		// 窗口被激活（切回游戏）
		UE_LOG(LogTemp, Log, TEXT("游戏窗口获得焦点"));
	}
	else
	{
		// 窗口失去焦点（切出去）
		UE_LOG(LogTemp, Log, TEXT("游戏窗口失去焦点"));
	}
}

