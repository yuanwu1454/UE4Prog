// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "MyLogging.h"
#include "MyViewManager.h"
#include "SubSystem/MyDynamicEngineSubsystem.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
#include "UI/SlateEventsHelper.h"
#include "Test/MySlateWidget.h"
#include "Base/StateMachine/GlobalStateMachine.h"
#include "GameFramework/WorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Startup/StartupGameMode.h"

#include "LuaCore.h"
#include "UnLua.h"
#include "UnLuaEx.h"

void UMyGameInstance::Init()
{

	// 创建全局状态机
	const UClass* SMClass = LoadClass<UObject>(nullptr, *GlobalStateMachineAsset.ToString());
	if (SMClass)
	{
		GlobalStateMachine = NewObject<UGlobalStateMachine>(this, SMClass);
		GlobalStateMachine->Start();
	}
	// 可以设置平台信息
	
	bAllSubsystemFinishInit = false;
	Super::Init();
	bAllSubsystemFinishInit = true;
	
	FCoreDelegates::OnHandleSystemError.AddUObject(this, &UMyGameInstance::OnHandleSystemError);
	FCoreDelegates::OnShutdownAfterError.AddUObject(this, &UMyGameInstance::OnShutdownAfterError);

	

	TickDelegate = FTickerDelegate::CreateUObject(this, &UMyGameInstance::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);

	FSlateEventsHelper::Get().Initialize();
	

	// NtySubsystemsStartUp();

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

void UMyGameInstance::OnStart()
{
	Super::OnStart();

	if (UGlobalStateMachine* GlobalSM = UGlobalStateMachine::Get(this))
	{
		UGlobalEntryStateEventData* EventData = NewObject<UGlobalEntryStateEventData>();
		GlobalSM->TransferGlobalState(EGlobalStateType::Entry, EventData);
	}
}

bool UMyGameInstance::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Out)
{
	return Super::Exec(InWorld, Cmd, Out);
}

#if WITH_EDITOR
FGameInstancePIEResult UMyGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer,
                                                                      const FGameInstancePIEParameters& Params)
{
	bool bEditorStartupFromStartupWorld = false;
	if (UWorld* World = GetWorld())
	{
		if (World && World->GetWorldSettings() && World->GetWorldSettings() && World->GetWorldSettings()->DefaultGameMode)
		{
			bEditorStartupFromStartupWorld = World->GetWorldSettings()->DefaultGameMode->IsChildOf(AStartupGameMode::StaticClass());
		}
	}
	if (UGlobalStateMachine* GlobalSM = UGlobalStateMachine::Get(this))
	{
		if (bEditorStartupFromStartupWorld)
		{
			// 正常状态机流程
			UGlobalEntryStateEventData* EventData = NewObject<UGlobalEntryStateEventData>();
			EventData->DebugMessage = TEXT("Editor enter entry");
			GlobalSM->TransferGlobalState(EGlobalStateType::Entry, EventData);
		}
		else
		{
			// 执行完初始化
			UGlobalInitStateEventData* Data = NewObject<UGlobalInitStateEventData>();
			Data->DebugMessage = TEXT("Editor enter init");
			Data->bNeedQuickFinishAndWait = true;
			Data->bInitForStartup = true;
			GlobalSM->TransferGlobalState(EGlobalStateType::Init, Data);
		}
	}
	return Super::StartPlayInEditorGameInstance(LocalPlayer, Params);
}
#endif


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

void UMyGameInstance::SetOpenLobbyMsgTips(const FText& Msg)
{
	OpenLobbyMsgTips = Msg;
}

void UMyGameInstance::ClearOpenLobbyTips()
{
	OpenLobbyMsgTips = FText::GetEmpty();
}

void UMyGameInstance::ReleaseCachedObject()
{
	MyGlobalsInstance = nullptr;
}

void UMyGameInstance::OnHandleSystemError()
{
	UE_LOG(LogPM, Error, TEXT("==== OnHandleSystemError ==="));
	lua_State* L = UnLua::GetState();
	if (L)
	{
		luaL_traceback(L, L, "", 0);
		FString LuaInfo = UTF8_TO_TCHAR(lua_tostring(L,-1));
		lua_pop(L, 1);
		UE_LOG(LogPM, Log, TEXT("CrashCallback lua traceback\n %s"), UTF8_TO_TCHAR(lua_tostring(L,-1)));
		// FCoreDelegates::LogImportantKeyValue.Broadcast(TEXT("LuaTraceback"), LuaInfo);
	}
	UE_LOG(LogPM, Error, TEXT("=== Critical error: ===\n %s"), GErrorHist);
}


// 自动打印当前代码的调用路径，帮你快速定位 BUG 发生在哪个文件、哪一行、哪个函数。
void UMyGameInstance::OnShutdownAfterError()
{
	TArray<FProgramCounterSymbolInfo> SymbolInfo = FGenericPlatformStackWalk::GetStack(0);
	FString StackInfo = "";
	for (FProgramCounterSymbolInfo Info : SymbolInfo)
	{
		StackInfo += FString::Format(TEXT("File: {0} {1} {2} \n"), {FStringFormatArg(Info.Filename), FStringFormatArg(Info.LineNumber), FStringFormatArg(Info.FunctionName)});
	}

	UE_LOG(LogPM, Error, TEXT("Statck Info: \n%s"), *StackInfo);

	FGenericPlatformProcess::ConditionalSleep([&]() { return true; }, 1);
}
