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

	// ======================
	// Draw2DLine 标准用法
	// ======================

	// 1. 画一条 屏幕中间 横线（白色）
	Draw2DLine(
		200,        // X1
		300,        // Y1
		600,        // X2
		300,        // Y2
		FColor::White // 颜色
	);

	// 2. 画一条 红色竖线
	Draw2DLine(500, 100, 500, 500, FColor::Red);

	// 3. 半透明蓝色线（A=100 透明）
	Draw2DLine(100, 100, 800, 500, FColor(0, 0, 255, 100));


	// 获取当前视口（窗口）的宽度和高度
	float ScreenWidth = Canvas->SizeX;   // 窗口宽度
	float ScreenHeight = Canvas->SizeY;  // 窗口高度

	// 你可以打印出来验证
	// UE_LOG(LogTemp, Log, TEXT("窗口宽：%f，高：%f"), ScreenWidth, ScreenHeight);

	// 3. 半透明蓝色线（A=100 透明）
	Draw2DLine(0, 0, ScreenWidth, ScreenHeight, FColor(0, 0, 255, 100));

	// 1. 获取角色位置（起点）
	FVector StartLoc = GetOwningPlayerController()->GetPawn()->GetActorLocation();

	// 2. 往前 1000 米的位置（终点）
	FVector EndLoc = StartLoc + GetOwningPlayerController()->GetPawn()->GetActorForwardVector() * 1000.0f;

	// 3. 画一条红色 3D 线
	Draw3DLine(StartLoc, EndLoc, FColor::Red);

	// ==========================
	// 案例1：基础绘制（默认字体、白色、左上角）
	// ==========================
	FString BasicText = TEXT("Hello UE4.27 HUD");
	FLinearColor White = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f); // RGBA 0~1
	float X = 20.0f; // 左上角 X
	float Y = 20.0f; // 左上角 Y
	DrawText(BasicText, White, X, Y);


	// ==========================
	// 案例2：彩色文字 + 自定义缩放
	// ==========================
	FString ColorText = TEXT("红色放大文字");
	FLinearColor Red = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
	DrawText(ColorText, Red, 20, 60, nullptr, 2.0f); // 缩放 2 倍


	// ==========================
	// 案例3：半透明蓝色文字
	// ==========================
	FString TransText = TEXT("半透明蓝色");
	FLinearColor TransBlue = FLinearColor(0.1f, 0.3f, 1.0f, 0.5f); // A=0.5 半透明
	DrawText(TransText, TransBlue, 20, 120, nullptr, 1.5f);
}

void AKingHUD::DrawActorOverlays(FVector Viewpoint, FRotator ViewRotation)
{
	Super::DrawActorOverlays(Viewpoint, ViewRotation);


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
