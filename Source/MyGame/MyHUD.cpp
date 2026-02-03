// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();
	AsyncCreateMainUI();
}

void AMyHUD::BeginDestroy()
{
	Super::BeginDestroy();
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