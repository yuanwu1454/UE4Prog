// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyGameGameModeBase.h"

#include "MyHUD.h"

AMyGameGameModeBase::AMyGameGameModeBase()
{
	// 核心：将HUDClass赋值为你的自定义HUD类
	HUDClass = AMyHUD::StaticClass();
}
