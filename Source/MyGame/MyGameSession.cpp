// Fill out your copyright notice in the Description page of Project Settings.

// MyGameSession.cpp
#include "MyGameSession.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

// Sets default values
AMyGameSession::AMyGameSession()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyGameSession::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyGameSession::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyGameSession::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
}

void AMyGameSession::NotifyLogout(const APlayerController* PC)
{
    Super::NotifyLogout(PC);
    // 玩家登出：取消注册
}