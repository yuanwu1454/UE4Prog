// Fill out your copyright notice in the Description page of Project Settings.


#include "KingGameMode.h"

#include "KingHUD.h"
#include "KingPlayerController.h"
#include "KingPlayerState.h"

AKingGameMode::AKingGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AKingPlayerController::StaticClass();
	HUDClass = AKingHUD::StaticClass();
	PlayerStateClass = AKingPlayerState::StaticClass();
}
