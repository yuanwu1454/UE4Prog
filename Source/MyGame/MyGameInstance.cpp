// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "UI\SlateEventsHelper.h"
#include "Test/MySlateWidget.h"

void UMyGameInstance::Init()
{
	Super::Init();

	FSlateEventsHelper::Get().Initialize();
}

void UMyGameInstance::Shutdown()
{
	Super::Shutdown();

	SMySlateWidget::GMySlateWidgetRoot.Reset();
	FSlateEventsHelper::Get().Shutdown();
}
