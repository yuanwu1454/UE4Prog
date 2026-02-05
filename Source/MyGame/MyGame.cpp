// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyGame.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FMyGameModule, MyGame, "MyGame" );

void FMyGameModule::StartupModule()
{
	IModuleInterface::StartupModule();

	UE_LOG(LogTemp, Log,TEXT("MyGame StartupModule"))
	
}

void FMyGameModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log,TEXT("MyGame ShutdownModule"))
	IModuleInterface::ShutdownModule();

}
