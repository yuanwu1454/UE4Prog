// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGlobals.h"

#include "MyGameInstance.h"
#include "Engine/GameEngine.h"

UMyGlobals& UMyGlobals::Get()
{
#if WITH_EDITOR
	if (GEngine->GameSingleton)
	{
		return *static_cast<UMyGlobals*>(GEngine->GameSingleton);
	}
	else
	{
		FString FixedGlobalsClassName = TEXT("/Game/PaperMan/Core/BP_Globals.BP_Globals_C");
		UClass *GlobalsClass = LoadClass<UObject>(nullptr, *FixedGlobalsClassName);
		UMyGlobals* GlobslObj =  NewObject<UMyGlobals>(GEngine, GlobalsClass);
		GEngine->GameSingleton = GlobslObj;
		return *GlobslObj;
	}
#else
	if (UMyGlobals* Globals = SafeGet())
	{
		return *Globals;
	}
	else
	{
		return *NewObject<UMyGlobals>();
	}
#endif
}
UMyGlobals* UMyGlobals::SafeGet()
{
	if (const UGameEngine* Engine = Cast<UGameEngine>(GEngine))
	{
		if(const UMyGameInstance* GameInstance = Cast<UMyGameInstance>(Engine->GameInstance))
		{
			return GameInstance->MyGlobalsInstance;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Retrive MyGlobals faild!GameInstance invalid!"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Retrive MyGlobals when GEngine is null"))
	}
	return nullptr; 
}
