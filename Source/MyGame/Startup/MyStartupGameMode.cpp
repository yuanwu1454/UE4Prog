// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStartupGameMode.h"


AMyStartupGameMode::AMyStartupGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bStartPlayersAsSpectators = true;
}

void AMyStartupGameMode::StartPlay()
{
	Super::StartPlay();
}

bool AMyStartupGameMode::CheckUpdate()
{
	return false;
}

bool AMyStartupGameMode::CheckNotice()
{
	return false;
}

bool AMyStartupGameMode::CheckFirstPlay()
{
	return false;
}

void AMyStartupGameMode::CheckLastLoginServer()
{
}

void AMyStartupGameMode::ShowLogin()
{
}
