// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "SubSystem/MyDynamicEngineSubsystem.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
#include "UI\SlateEventsHelper.h"
#include "Test/MySlateWidget.h"

void UMyGameInstance::Init()
{
	Super::Init();

	NtySubsystemsStartUp();

	SetupGlobalsCfgObject();
	
	FSlateEventsHelper::Get().Initialize();

	
	// 加载你的模块（模块名必须正确）
	FModuleManager::Get().LoadModule("MyGame");
	UMyDynamicEngineSubsystem* Sub = GEngine->GetEngineSubsystem<UMyDynamicEngineSubsystem>();
	if (Sub)
	{
		Sub->TestDynamicSubsystem();
	}
}

void UMyGameInstance::Shutdown()
{
	NtySubsystemsShutDown();
	CachedRegistedSubsystemArray.Empty();
	
	Super::Shutdown();

	SMySlateWidget::GMySlateWidgetRoot.Reset();
	FSlateEventsHelper::Get().Shutdown();
}

void UMyGameInstance::SetupGlobalsCfgObject()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UPMGameInstance_SetupGlobalsCfgObject);

	if (MyGlobalsInstance == nullptr && MyGlobalsClass.ToString().Len() > 0)
	{
		const UClass* TmpClass = LoadClass<UObject>(nullptr, *MyGlobalsClass.ToString());
		if (MyGlobalsClass)
		{
			UE_LOG(LogTemp, Log, TEXT("Setup Globals Cfg obje!"));
			MyGlobalsInstance = NewObject<UMyGlobals>(this, TmpClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("can't load pmglobals class %s"), *MyGlobalsClass.ToString());
		}
	}
}

void UMyGameInstance::NtySubsystemsStartUp()
{
	for (auto RegistedSubsystem : CachedRegistedSubsystemArray)
	{
		RegistedSubsystem->OnStartUp();
	}
}

void UMyGameInstance::NtySubsystemsShutDown()
{
	for (auto RegistedSubsystem : CachedRegistedSubsystemArray)
	{
		RegistedSubsystem->OnShutDown();
	}
}

void UMyGameInstance::RegisterMyGameSubSystemBase(UMyGameInstanceSubsystem* Subsystem)
{
	if (Subsystem)
	{
		UE_LOG(LogTemp, Log, TEXT("PMGameinstanceSubsystem %s Regist!"), *Subsystem->GetName());
		CachedRegistedSubsystemArray.Add(Subsystem);
	}
}
