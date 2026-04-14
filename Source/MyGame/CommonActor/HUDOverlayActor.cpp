// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDOverlayActor.h"

#include "Engine/Canvas.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"


// Sets default values
AHUDOverlayActor::AHUDOverlayActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHUDOverlayActor::BeginPlay()
{
	Super::BeginPlay();

	// 只在开始时加入一次！完全符合你的想法！
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetHUD())
	{
		PC->GetHUD()->AddPostRenderedActor(this);
	}

	
	
}

void AHUDOverlayActor::PostRenderFor(APlayerController* PC, UCanvas* Canvas, FVector CameraPosition, FVector CameraDir)
{
	if (!Canvas) return;


	if (bShowInfo)
	{
		// 把3D位置转屏幕
		FVector2D ScreenPos;
		if (PC->ProjectWorldLocationToScreen(GetActorLocation(), ScreenPos))
		{
			// 在头顶画文字
			Canvas->SetDrawColor(FColor::Yellow);
			Canvas->DrawText(GEngine->GetSmallFont(), TEXT("测试Actor正常工作!"), ScreenPos.X, ScreenPos.Y);
		}
	}
}

