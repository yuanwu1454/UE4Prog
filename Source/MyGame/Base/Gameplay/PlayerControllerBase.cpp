// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"


void APlayerControllerBase::EnterGameOnlyInputMode()
{
	FInputModeGameOnly GameOnlyMode;
	SetInputMode(GameOnlyMode);
	bShowMouseCursor = false;
}

void APlayerControllerBase::EnterUIOnlyInputMode()
{
	FInputModeUIOnly UIOnlyMode;
	SetInputMode(UIOnlyMode);
	bShowMouseCursor = true;

}