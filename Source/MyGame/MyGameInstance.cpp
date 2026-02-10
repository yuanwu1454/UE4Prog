// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Test/MySlateWidget.h"

void UMyGameInstance::Shutdown()
{
	Super::Shutdown();

	// SMySlateWidget::GMySlateWidgetRoot.Reset();
}
