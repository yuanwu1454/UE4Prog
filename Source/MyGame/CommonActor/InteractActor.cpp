// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractActor.h"

#include "Camera/CameraComponent.h"


// ------------------------------
// 组件
// ------------------------------
UInteractComponent::UInteractComponent()
{
	UE_LOG(LogTemp, Warning, TEXT("[组件] 构造函数"));
}

void UInteractComponent::InitializeComponent()
{
	Super::InitializeComponent();
	UE_LOG(LogTemp, Warning, TEXT("[组件] InitializeComponent"));
}

void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[组件] BeginPlay"));
}

void UInteractComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	UE_LOG(LogTemp, Warning, TEXT("[组件] OnComponentDestroyed"));
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

// Sets default values
AInteractActor::AInteractActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	

	// ✅ 第一步：创建空场景组件，设为根节点（必须第一个创建）
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene; // 绑定为Actor的根组件

	// ✅ 第二步：创建StaticMesh，挂在RootScene下（平级组件1）
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootScene); // 挂载到根，不是相机/其他组件

	// ✅ 第三步：创建CameraComponent，也挂在RootScene下（平级组件2）
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootScene); // 同样挂载到根，和StaticMesh平级

	// 可选：给相机一个初始位置，避免和网格体重叠
	CameraComponent->SetRelativeLocation(FVector(0, 0, 150.0f));
	CameraComponent->bAutoActivate = true;

	
	// 创建组件
	InteractComp = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComp"));
}

// Called when the game starts or when spawned
void AInteractActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("=== 5. BeginPlay 游戏开始 ==="));
}

// ====================== 鼠标点击 ======================
void AInteractActor::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);
	UE_LOG(LogTemp, Warning, TEXT("[点击] %s 被鼠标左键点击了！"), *GetName());
}

void AInteractActor::NotifyActorOnReleased(FKey ButtonReleased)
{
	Super::NotifyActorOnReleased(ButtonReleased);
	UE_LOG(LogTemp, Warning, TEXT("[松开] %s 鼠标左键松开了"), *GetName());
}

// ====================== 鼠标悬停 ======================
void AInteractActor::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	UE_LOG(LogTemp, Warning, TEXT("[悬停开始] 鼠标移到了 %s 身上"), *GetName());
}

void AInteractActor::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	UE_LOG(LogTemp, Warning, TEXT("[悬停结束] 鼠标离开了 %s"), *GetName());
}

// ====================== 视角成为相机目标 ======================
void AInteractActor::BecomeViewTarget(APlayerController* PC)
{
	Super::BecomeViewTarget(PC);
	UE_LOG(LogTemp, Warning, TEXT("[视角] 我成为了相机观察目标：%s"), *GetName());
}

void AInteractActor::EndViewTarget(APlayerController* PC)
{
	Super::EndViewTarget(PC);
	UE_LOG(LogTemp, Warning, TEXT("[视角] 我不再是相机观察目标：%s"), *GetName());
}

// 构造脚本（编辑器/游戏都会跑）
// 本质上是为了服务编辑器而创作的。 点击蓝图上的编译就会运行下，打开蓝图也会编译下
void AInteractActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UE_LOG(LogTemp, Warning, TEXT("=== 3. OnConstruction 构造脚本 ==="));
}

// 组件全部初始化完
void AInteractActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogTemp, Warning, TEXT("=== 4. PostInitializeComponents (组件全部初始化完成) ==="));
}

// 销毁
void AInteractActor::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("=== 6. Actor Destroyed ==="));
	Super::Destroyed();
}

// ====================== 碰撞重叠（穿模/进入/离开） ======================
void AInteractActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("[重叠开始] %s 碰到了：%s"), *GetName(), *OtherActor->GetName());
}

void AInteractActor::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	UE_LOG(LogTemp, Warning, TEXT("[重叠结束] %s 离开了：%s"), *GetName(), *OtherActor->GetName());
}

// ====================== 重置 ======================
void AInteractActor::Reset()
{
	Super::Reset();
	UE_LOG(LogTemp, Warning, TEXT("[重置] %s 被重置了"), *GetName());
}


// GameAndUI（最常用）
// Game Only（也能触发！）
// PlayerController 开启
// bEnableClickEvents = true（点击）
// bEnableMouseOverEvents = true（悬停）
// Actor 必须有碰撞体
// StaticMeshComponent / CollisionComponent
// Collision 设置为：Query Only 或 Query and Physics

// UI Only 模式 不触发


// BecomeViewTarget
// EndViewTarget
// PC->SetViewTarget(MyActor);

// 先加载，后创建，
// 组件注册排前面。
// 构造脚本跑一遍，
// 组件初始化再连线。
// 最后 BeginPlay 出现，
// 游戏逻辑才开演。


// Actor 的构造函数与OnConstruction
// 1. 构造函数 Actor ()
// C++ 层面的构造
// 只跑一次
// 用来搭骨架：创建组件、设默认值、设 Root
// 游戏运行 + 编辑器都用它

// 2. OnConstruction
// 引擎层面的 “重建” 回调
// 可以反复执行 N 次
// 主要为了：
// 编辑器里拖动物体
// 改 Details 面板参数
// 打开关卡
// 移动、旋转、缩放
// → 立刻看到预览效果