// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "MyViewManager.h"
#include "SubSystem/MyDynamicEngineSubsystem.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
#include "UI/SlateEventsHelper.h"
#include "Test/MySlateWidget.h"
#include "Base/StateMachine/GlobalStateMachine.h"
#include "Kismet/GameplayStatics.h"

void UMyGameInstance::Init()
{
	Super::Init();


	TickDelegate = FTickerDelegate::CreateUObject(this, &UMyGameInstance::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);

	FSlateEventsHelper::Get().Initialize();
	
	// 创建全局状态机
    const UClass* SMClass = LoadClass<UObject>(nullptr, *GlobalStateMachineAsset.ToString());
    if (SMClass)
    {
    	GlobalStateMachine = NewObject<UGlobalStateMachine>(this, SMClass);
    	GlobalStateMachine->Start();
    }
	
	NtySubsystemsStartUp();

	SetupGlobalsCfgObject();
	
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

	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	NtySubsystemsShutDown();
	CachedRegisterSubsystemArray.Empty();
	
	Super::Shutdown();

	if (GlobalStateMachine)
	{
		GlobalStateMachine->End();
	}
	
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
	for (auto RegistedSubsystem : CachedRegisterSubsystemArray)
	{
		RegistedSubsystem->OnStartUp();
	}
}

void UMyGameInstance::NtySubsystemsShutDown()
{
	for (auto RegistedSubsystem : CachedRegisterSubsystemArray)
	{
		RegistedSubsystem->OnShutDown();
	}
}

void UMyGameInstance::RegisterMyGameSubSystemBase(UMyGameInstanceSubsystem* Subsystem)
{
	if (Subsystem)
	{
		UE_LOG(LogTemp, Log, TEXT("PMGameinstanceSubsystem %s Regist!"), *Subsystem->GetName());
		CachedRegisterSubsystemArray.Add(Subsystem);
	}
}

bool UMyGameInstance::Tick(float DeltaSeconds)
{
	if (GlobalStateMachine)
	{
		GlobalStateMachine->ProcessTick(DeltaSeconds);
	}
	return true;
}


void UMyGameInstance::GotoLoginScene()
{
	UE_LOG(LogTemp, Log, TEXT("UMyGameInstance::GotoLoginScene Cur World %s"), GetWorld() ? *GetWorld()->GetName() : TEXT("ERR_NOWorld"));
	UGlobalStateMachine::Get(this)->TransferGlobalState(EGlobalStateType::Init, nullptr);
}

void UMyGameInstance::GotoLobbyScene()
{
	const UWorld* World = GetWorld();
	UE_LOG(LogTemp, Log, TEXT("UMyGameInstance::GotoLobbyScene Cur World %s"), World ? *World->GetName() : TEXT("ERR_NOWorld"));
	FString tempName = UGameplayStatics::GetCurrentLevelName(this);
	UMyViewManager::Get(this)->CloseAllPage();
	UGlobalStateMachine::Get(this)->TransferGlobalState(EGlobalStateType::Lobby, nullptr);
}