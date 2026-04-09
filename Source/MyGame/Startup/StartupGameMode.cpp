// Fill out your copyright notice in the Description page of Project Settings.


#include "StartupGameMode.h"

#include "StateMachine/GlobalStateMachine.h"


AStartupGameMode::AStartupGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bStartPlayersAsSpectators = true;
}

void AStartupGameMode::StartPlay()
{
	Super::StartPlay();
}

bool AStartupGameMode::CheckUpdate()
{
	return false;
}

bool AStartupGameMode::CheckNotice()
{
	return false;
}

bool AStartupGameMode::CheckFirstPlay()
{
	return false;
}

void AStartupGameMode::CheckLastLoginServer()
{
}

void AStartupGameMode::ShowLogin()
{
}
