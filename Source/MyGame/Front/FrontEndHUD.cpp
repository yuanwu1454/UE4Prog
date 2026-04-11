// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontEndHUD.h"

#include "MyViewManager.h"
#include "Types.h"

void AFrontEndHUD::BeginPlay()
{
	Super::BeginPlay();
	
	UMyViewManager::Get(this)->OpenPage(FUINames::EM_NavigationPage);
}

void AFrontEndHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
