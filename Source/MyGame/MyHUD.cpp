// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "CanvasItem.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/Canvas.h"
#include "Engine/StreamableManager.h"
#include "UObject/ConstructorHelpers.h"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();
	// AsyncCreateMainUI();
}

void AMyHUD::BeginDestroy()
{
	Super::BeginDestroy();
}

AMyHUD::AMyHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}


void AMyHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

void AMyHUD::AsyncCreateMainUI()
{
	if (MainUISoftClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("MainUISoftClass is not assigned!"));
		return;
	}

	// 异步加载类（避免主线程卡顿）
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	StreamableManager.RequestAsyncLoad(MainUISoftClass, FStreamableDelegate::CreateUObject(this, &AMyHUD::OnWidgetLoaded));
}

// 加载完成后的回调函数
void AMyHUD::OnWidgetLoaded()
{
	// 转换为UUserWidget类
	UClass* WidgetClass = MainUISoftClass.TryLoadClass<UUserWidget>();
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load WidgetClass from SoftPath!"));
		return;
	}

	// 创建并显示UI
	UUserWidget* MainUIWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
	if (MainUIWidget)
	{
		MainUIWidget->AddToViewport();
	}
}