// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInputProcessor.h"

#include "MyViewManager.h"
#include "Framework/Application/SlateApplication.h"

FMyInputProcessor::FMyInputProcessor()
{
	ViewManager = nullptr;
}

FMyInputProcessor::~FMyInputProcessor()
{
	ViewManager = nullptr;
}
void FMyInputProcessor::SetViewManager(UMyViewManager* InViewManager)
{
	ViewManager = InViewManager;
}


void FMyInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}

bool FMyInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	// 你的全局按键逻辑
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		UE_LOG(LogTemp, Log, TEXT("全局按下 ESC"));
	}

	return false;
}

bool FMyInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	bool bRet = false;
	if (!InKeyEvent.GetKey().IsValid())
	{
		return bRet;
	}
	if(!bRet)
	{
		if(ViewManager)
		{
			bRet = ViewManager->ProcessKeyEvent(InKeyEvent.GetKey(), IE_Released);
		}
	}
	return bRet;
}

bool FMyInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	return false;
}

bool FMyInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	return false;
}

bool FMyInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	return false;
}

bool FMyInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	return false;
}

bool FMyInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& WheelEvent, const FPointerEvent* GestureEvent)
{
	return false;
}