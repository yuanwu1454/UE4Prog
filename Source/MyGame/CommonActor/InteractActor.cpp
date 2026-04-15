// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractActor.h"

#include "Camera/CameraComponent.h"


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
}

// Called when the game starts or when spawned
void AInteractActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[Actor] 出生：%s"), *GetName());
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