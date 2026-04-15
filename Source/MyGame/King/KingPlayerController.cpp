// Fill out your copyright notice in the Description page of Project Settings.


#include "KingPlayerController.h"

#include "CommonActor/InteractActor.h"
#include "Kismet/KismetSystemLibrary.h"

AKingPlayerController::AKingPlayerController(const FObjectInitializer& ObjectInitializer)
{
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AKingPlayerController::BeginPlay()
{
	Super::BeginPlay();
	OriginalViewTarget = GetViewTarget(); // 记录默认视角
}

void AKingPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 绑定 C 键
	InputComponent->BindKey(FKey("C"), IE_Pressed, this, &AKingPlayerController::Press_C_Button);
}

void AKingPlayerController::Press_C_Button()
{

	// 如果已经在看某个InteractActor → 切回去
	if (CurrentFocusedActor != nullptr)
	{
		SetViewTarget(OriginalViewTarget);
		CurrentFocusedActor = nullptr;
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("切回玩家视角"));
		return;
	}

	
	
	// 1. 发射射线
	FHitResult Hit;
	if (!LineTraceFromScreenCenter(Hit))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("未击中任何物体"));
		return;
	}

	// 2. 判断是不是你的 InteractActor
	AActor* HitActor = Hit.GetActor();
	if (!HitActor || !HitActor->IsA<AInteractActor>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("击中的不是 InteractActor"));
		return;
	}

	// 3. 切换视角到这个 Actor
	SetViewTarget(HitActor);
	CurrentFocusedActor= HitActor;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		FString::Printf(TEXT("已切换视角到：%s"), *HitActor->GetName()));
}

bool AKingPlayerController::LineTraceFromScreenCenter(FHitResult& HitResult)
{
	// 屏幕分辨率大小
	int32 X, Y;
	GetViewportSize(X, Y);

	// 屏幕中心点
	FVector2D ScreenCenter = FVector2D(X / 2.f, Y / 2.f);

	FVector Start, Dir;
	DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, Start, Dir);

	FVector End = Start + Dir * 5000.0f;

	// 射线检测
	return UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		Start,
		End,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame, // 显示射线（调试用）
		HitResult,
		true
	);
}
