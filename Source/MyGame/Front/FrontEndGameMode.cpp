// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontEndGameMode.h"

#include "FrontEndGameState.h"
#include "FrontEndHUD.h"
#include "FrontEndPlayerController.h"


AFrontEndGameMode::AFrontEndGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AFrontEndPlayerController::StaticClass();
	HUDClass = AFrontEndHUD::StaticClass();
	GameStateClass = AFrontEndGameState::StaticClass();
}


void AFrontEndGameMode::StartPlay()
{
	Super::StartPlay();
}

void AFrontEndGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void AFrontEndGameMode::StartToLeaveMap()
{
	Super::StartToLeaveMap();
}
