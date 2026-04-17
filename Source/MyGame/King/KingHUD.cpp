// Fill out your copyright notice in the Description page of Project Settings.


#include "KingHUD.h"

#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"


AKingHUD::AKingHUD(const FObjectInitializer& ObjectInitializer)
{
	bShowOverlays = true;
}

void AKingHUD::DrawHUD()
{
	Super::DrawHUD(); // 必须先调用父类
}

void AKingHUD::DrawActorOverlays(FVector Viewpoint, FRotator ViewRotation)
{
	Super::DrawActorOverlays(Viewpoint, ViewRotation);
}

void AKingHUD::NotifyHitBoxClick(FName BoxName)
{
	Super::NotifyHitBoxClick(BoxName);
	if (BoxName == Btn_Start)
    {
    	UE_LOG(LogTemp, Log, TEXT("=== 开始游戏 ==="));
    }
    else if (BoxName == Btn_Exit)
    {
    	UE_LOG(LogTemp, Log, TEXT("=== 退出游戏 ==="));
    }
}

void AKingHUD::NotifyHitBoxBeginCursorOver(FName BoxName)
{
	Super::NotifyHitBoxBeginCursorOver(BoxName);
	UE_LOG(LogTemp, Log, TEXT("鼠标进入：%s"), *BoxName.ToString());
}

void AKingHUD::NotifyHitBoxEndCursorOver(FName BoxName)
{
	Super::NotifyHitBoxEndCursorOver(BoxName);
	UE_LOG(LogTemp, Log, TEXT("鼠标离开：%s"), *BoxName.ToString());
}

void AKingHUD::TestHUD()
{
	if(bShowActorOverlayInfo)
	{
		APlayerController* PlayerController = GetOwningPlayerController();
		// 遍历这一帧所有可见的 Actor
		for (AActor* Actor : PostRenderedActors)
		{
			if (!Actor) continue;

			// 1. 获取Actor世界位置
			FVector ActorLoc = Actor->GetActorLocation();

			// 2. 3D坐标转屏幕坐标
			FVector2D ScreenPos;
			if (PlayerController->ProjectWorldLocationToScreen(ActorLoc, ScreenPos))
			{
				// 3. 在屏幕上绘制文字（Actor名字）
				DrawText(
					Actor->GetName(),          // 文字内容
					FColor::Yellow,            // 颜色
					ScreenPos.X,               // X
					ScreenPos.Y,               // Y
					nullptr,                   // 字体
					1.0f                       // 缩放
				);
			}
		}	
	}

	// ==============================================
	// 1. 添加按钮 → 这些都会存入 HitBoxMap
	// ==============================================
	AddHitBox(
		FVector2D(100, 100),	// 位置
		FVector2D(200, 50),		// 大小
		Btn_Start,				// 名字
		true					// 是否消费点击
	);

	AddHitBox(
		FVector2D(100, 200),
		FVector2D(200, 50),
		Btn_Exit,
		true
	);

	// ==============================================
	// 2. 引擎PostRender里会自动调用：
	//    UpdateHitBoxCandidates(鼠标坐标)
	//    → 自动填充 HitBoxHits
	// 你不需要手动调用！
	// ==============================================

	// ==============================================
	// 3. 你可以直接读取：HitBoxMap（所有按钮）
	// ==============================================
	// UE_LOG(LogTemp, Log, TEXT("本帧总共有 %d 个HitBox"), HitBoxMap.Num());

	// ==============================================
	// 4. 你可以直接读取：HitBoxHits（鼠标碰到的）
	// ==============================================
	if (AnyCurrentHitBoxHits())
	{
		UE_LOG(LogTemp, Log, TEXT("鼠标碰到了 %d 个按钮"), HitBoxHits.Num());

		// 遍历所有碰到的 HitBox
		for (FHUDHitBox* Hit : HitBoxHits)
		{
			UE_LOG(LogTemp, Log, TEXT("碰到按钮：%s"), *Hit->GetName().ToString());
		}
	}

	// ==============================================
	// 5. 读取：HitBoxesOver（鼠标正悬浮的）
	// ==============================================
	for (FName Name : HitBoxesOver)
	{
		// UE_LOG(LogTemp, Log, TEXT("鼠标悬浮：%s"), *Name.ToString());
	}

	// ==============================================
	// 6. 画按钮文字
	// ==============================================
	DrawText(TEXT("开始游戏"), FLinearColor::White, 110, 110);
	DrawText(TEXT("退出游戏"), FLinearColor::White, 110, 210);
}
