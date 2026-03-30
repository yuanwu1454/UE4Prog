// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "SubSystem/MyDynamicEngineSubsystem.h"
#include "UI\SlateEventsHelper.h"
#include "Test/MySlateWidget.h"

void UMyGameInstance::Init()
{
	Super::Init();

	FSlateEventsHelper::Get().Initialize();

	// 加载你的模块（模块名必须正确）
	FModuleManager::Get().LoadModule("MyGame");
	UMyDynamicEngineSubsystem* Sub = GEngine->GetEngineSubsystem<UMyDynamicEngineSubsystem>();
	if (Sub)
	{
		Sub->TestDynamicSubsystem();
	}
}

void UMyGameInstance::Shutdown()
{
	// FModuleManager::Get().UnloadModule("MyGame");
	Super::Shutdown();

	SMySlateWidget::GMySlateWidgetRoot.Reset();
	FSlateEventsHelper::Get().Shutdown();
}
