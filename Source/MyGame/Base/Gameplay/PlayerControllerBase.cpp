// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"


void APlayerControllerBase::EnterGameOnlyInputMode()
{
	FInputModeGameOnly GameOnlyMode;
	SetInputMode(GameOnlyMode);
	bShowMouseCursor = false;

	UE_LOG(LogTemp, Log, L"[Input] 切换到：GameOnly");
}

void APlayerControllerBase::EnterUIOnlyInputMode()
{
	FInputModeUIOnly UIOnlyMode;
	UIOnlyMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	UIOnlyMode.SetWidgetToFocus(nullptr);
	SetInputMode(UIOnlyMode);
	bShowMouseCursor = true;

	UE_LOG(LogTemp, Log, L"[Input] 切换到：UIOnly");

}


void APlayerControllerBase::EnterGameAndUIInputMode()
{
	// 【重要】这个模式才能让 HUD HitBox 正常工作！
	FInputModeGameAndUI GameAndUIMode;
	GameAndUIMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	GameAndUIMode.SetHideCursorDuringCapture(false);
	SetInputMode(GameAndUIMode);
	bShowMouseCursor = true;

	UE_LOG(LogTemp, Log, L"[Input] 切换到：GameAndUI (HitBox可用)");
}

void APlayerControllerBase::GameOnly()
{
	EnterGameOnlyInputMode();
}

void APlayerControllerBase::UIOnly()
{
	EnterUIOnlyInputMode();
}
void APlayerControllerBase::GameAndUI()
{
	EnterGameAndUIInputMode();
}