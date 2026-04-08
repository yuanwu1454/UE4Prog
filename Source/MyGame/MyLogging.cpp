//###First Include

//###Engine Include
#include "MyLogging.h"
#include "UnLuaBase.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetStringLibrary.h"

DEFINE_LOG_CATEGORY(LogPM);
DEFINE_LOG_CATEGORY(LogPMGameMode);
DEFINE_LOG_CATEGORY(LogPMGameState);
DEFINE_LOG_CATEGORY(LogPMPlayerController);
DEFINE_LOG_CATEGORY(LogPMAkAudio);
DEFINE_LOG_CATEGORY(LogPMWeapon);
DEFINE_LOG_CATEGORY(LogCMachine);
DEFINE_LOG_CATEGORY(LogAIMachine);
DEFINE_LOG_CATEGORY(LogWpnMachine);
DEFINE_LOG_CATEGORY(LogFireSwitch);
DEFINE_LOG_CATEGORY(LogCyAbility);
DEFINE_LOG_CATEGORY(LogFireSwitcher);
DEFINE_LOG_CATEGORY(LogAtkSwitcher);
DEFINE_LOG_CATEGORY(LogHoldSwitcher);
DEFINE_LOG_CATEGORY(LogPMAI);
DEFINE_LOG_CATEGORY(LogPMCsv);
DEFINE_LOG_CATEGORY(LogPMDataCenter);
DEFINE_LOG_CATEGORY(LogPMOnline);
DEFINE_LOG_CATEGORY(LogPMUI);
DEFINE_LOG_CATEGORY(LogPMUITick);
DEFINE_LOG_CATEGORY(LogPMUIJoystick);
DEFINE_LOG_CATEGORY(LogPMUIAutoFire);
DEFINE_LOG_CATEGORY(LogPMUISceneMap);
DEFINE_LOG_CATEGORY(LogPMLoading);
DEFINE_LOG_CATEGORY(LogPMCharacterMovement);
DEFINE_LOG_CATEGORY(LogPMCamera);
DEFINE_LOG_CATEGORY(LogPMCharacter);
DEFINE_LOG_CATEGORY(LogPMPlayerState);
DEFINE_LOG_CATEGORY(LogPMGC);
DEFINE_LOG_CATEGORY(LogLDSdk);
DEFINE_LOG_CATEGORY(LogMSDKSdk);
DEFINE_LOG_CATEGORY(LogNotice);
DEFINE_LOG_CATEGORY(LogPufferSdk);
DEFINE_LOG_CATEGORY(LogDolphinSdk);
DEFINE_LOG_CATEGORY(LogCrashSightSdkSubSystem);

DEFINE_LOG_CATEGORY(LogAppsFlyerSdk);
// 数据埋点
DEFINE_LOG_CATEGORY(LogDataTracker);
DEFINE_LOG_CATEGORY(LogBattleReport);
DEFINE_LOG_CATEGORY(LogInGameVoice);
// 新手引导
DEFINE_LOG_CATEGORY(LogCyGuide);
DEFINE_LOG_CATEGORY(LogCyNoviceGuide);

// 任务
DEFINE_LOG_CATEGORY(LogCyTaskCore);
DEFINE_LOG_CATEGORY(LogCyGameTask);
DEFINE_LOG_CATEGORY(LogCyGuideTask);
DEFINE_LOG_CATEGORY(LogCyAchievement);

// 剧情
DEFINE_LOG_CATEGORY(Log_Sequence);

// 剧情
DEFINE_LOG_CATEGORY(Log_AVGEvent);

//伤害信息
DEFINE_LOG_CATEGORY(LogDamageInfo);

// 作弊
DEFINE_LOG_CATEGORY(LogCheat);

// Lua
DEFINE_LOG_CATEGORY(LogLua);

DEFINE_LOG_CATEGORY(Log_TssSdk);

DEFINE_LOG_CATEGORY(Log_SafeSDK);

DEFINE_LOG_CATEGORY(Log_Midas);

DEFINE_LOG_CATEGORY(Log_InternationPay);

DEFINE_LOG_CATEGORY(Log_LoginOfficial);

DEFINE_LOG_CATEGORY(Log_GVoiceNotify);

DEFINE_LOG_CATEGORY(LogSteamSdkSub)

DEFINE_LOG_CATEGORY(LogTLogHttp)

DEFINE_LOG_CATEGORY(LogWeGame)

DEFINE_LOG_CATEGORY(LogGCloudSdk);

DEFINE_LOG_CATEGORY(LogGameResUpdate)

// 反作弊
DEFINE_LOG_CATEGORY(LogACE)

DEFINE_LOG_CATEGORY(LogDsNetwork)

DEFINE_LOG_CATEGORY(LogWonderfulMomentEvent)

DEFINE_LOG_CATEGORY(LogOnlineSdk)

DEFINE_LOG_CATEGORY(LogCyStatAttributeSet)

DEFINE_LOG_CATEGORY(LogPandoraSdk)

DEFINE_LOG_CATEGORY(LogEsBrdSdk)

// 海外客服
DEFINE_LOG_CATEGORY(LogAIHelp)
//网络加速
DEFINE_LOG_CATEGORY(LogGRomeLink)

//Roguelike
DEFINE_LOG_CATEGORY(LogCyRoguelike)

// -1:代表禁用
int LogScreenErrCount = 0;
uint64 LogScrrenErrKey = 1112111;
FString ScreenLogStr;
FString LastScreenLogStr;

void Log_ScreenErr(const FString& LogString)
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (LogScreenErrCount < 3 && LogScreenErrCount >= 0)
	{
		if (LogScreenErrCount == 0)
		{
			ScreenLogStr += FString(TEXT("清除：ClearLogScreenError 开关：ToggleLogScreenError")); //-code parse
		}
		++LogScreenErrCount;
		ScreenLogStr += FString::Printf(TEXT("\n No.%d\n"), LogScreenErrCount);
		ScreenLogStr += LogString;
		GEngine->AddOnScreenDebugMessage(LogScrrenErrKey, 3600000.0f, FColor::Red, ScreenLogStr, false);
	}
	else if(LogScreenErrCount >= 3)
	{
		++LogScreenErrCount;
		LastScreenLogStr.Empty();
		LastScreenLogStr += ScreenLogStr;
		LastScreenLogStr += FString::Printf(TEXT("\n No.%d\n"), LogScreenErrCount);
		LastScreenLogStr += LogString;
		GEngine->AddOnScreenDebugMessage(LogScrrenErrKey, 3600000.0f, FColor::Red, LastScreenLogStr, false);
	}
#if !WITH_EDITOR
	ReportSystem::ReportData(ReportTarget::CrashSight, "CustomReport", "", TCHAR_TO_UTF8(*LogString));
#endif
#else
	ReportSystem::ReportData(ReportTarget::CrashSight, "CustomReport", "", TCHAR_TO_UTF8(*LogString));
#endif
	UE_LOG(LogPM, Log, TEXT("LogScreenContent: %s"), *LogString);
}


void Log_RunLuaFunc(const FString& LogString)
{
	lua_State* L = UnLua::GetState();
	if (LogString.Compare("") == 0)
	{
		UnLua::RunChunk(L, "GlobalFunc()");		
	}else
	{
		UnLua::RunChunk(L, TCHAR_TO_ANSI(*LogString));
	}
}


void Log_ScreenErr_Clear()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	GEngine->RemoveOnScreenDebugMessage(LogScrrenErrKey);
	if (LogScreenErrCount >= 0)
		LogScreenErrCount = 0;
	ScreenLogStr.Empty();
	LastScreenLogStr.Empty();
	ScreenLogStr = "";
#endif
}

void Log_ScreenErr_Toggle()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (LogScreenErrCount == -1)
		LogScreenErrCount = 0;
	else
	{
		Log_ScreenErr_Clear();
		LogScreenErrCount = -1;
	}
#endif
}

void AppendObjectLogString(FString& Result, const UActorComponent* InComponent)
{
	if (InComponent)
	{
		AppendObjectLogString(Result, InComponent->GetOwner());
		Result.Append("[C]:");
		Result.Append(GetNameSafe(InComponent));
		Result.Append(" ");
	}
}

void AppendObjectLogString(FString& Result, const AActor* InActor)
{
	if (InActor)
	{
		Result.Append(" [R]:");
		switch (InActor->GetLocalRole())
		{
		case ROLE_SimulatedProxy:
			Result.Append("S");
			break;
		case ROLE_AutonomousProxy:
			Result.Append("AP");
			break;
		case ROLE_Authority:
			Result.Append("A");
			break;
		default: ;
		}
		Result.Append(" [I]:");
		Result.Append(GetNameSafe(InActor->GetInstigator()));
		if (InActor->GetInstigator() != InActor->GetOwner())
		{
			Result.Append(" [O]:");
			Result.Append(GetNameSafe(InActor->GetOwner()));
		}
		Result.Append(" [A]:");
		Result.Append(GetNameSafe(InActor));
		Result.Append(" ");
		
		Result.Append(" [P]:");
		Result.Append(InActor->GetActorLocation().ToString());
		Result.Append(" ");
	}
}

void AppendObjectLogString(FString& Result, const USubsystem* InTarget)
{
	if(InTarget)
	{
		AppendObjectLogString(Result, InTarget->GetWorld());
	}
}

void AppendObjectLogString(FString& Result, const UUserWidget* InWidget)
{
	if(InWidget)
	{
		AppendObjectLogString(Result, InWidget->GetOwningPlayerPawn());
	}
}

void AppendObjectLogString(FString& Result, const UGameInstance* InTarget)
{
	if(InTarget)
	{
		AppendObjectLogString(Result, InTarget->GetWorld());
	}
}

void AppendObjectLogString(FString& Result, const UWorld* InTarget)
{
	if (InTarget)
	{
		Result.Append(" [M]:");
		switch (InTarget->GetNetMode())
		{
		case ENetMode::NM_DedicatedServer:
			Result.Append("DS");
			break;
		case ENetMode::NM_Client:
			Result.Append("C");
			break;
		case ENetMode::NM_Standalone:
			Result.Append("S");
			break;
		case ENetMode::NM_ListenServer:
			Result.Append("LS");
			break;
		default: ;
		}
		Result.Append(" ");
	}
}

static FAutoConsoleCommand ClearLogScreenError(
	TEXT("ClearLogScreenError"),
	TEXT("Clear Log Screen Error Message"),
	FConsoleCommandDelegate::CreateStatic(Log_ScreenErr_Clear)
);

static FAutoConsoleCommand ToggleLogScreenError(
	TEXT("ToggleLogScreenError"),
	TEXT("Toggle Log Screen Error Message"),
	FConsoleCommandDelegate::CreateStatic(Log_ScreenErr_Toggle)
);

static FAutoConsoleCommand PrintLogScreenError(
	TEXT("PrintLogScreenErr"),
	TEXT("Print Log Screen Error Message"),
	// FConsoleCommandDelegate::CreateStatic(Log_ScreenErr)
	FConsoleCommandDelegate::CreateLambda([]()
	{
		Log_ScreenErr("TestPrintLogScreenError");
	})
);


static FAutoConsoleCommandWithWorldAndArgs RunLuaFunc(
	TEXT("RunLuaFunc"),
	TEXT("Run Lua GlobalFunc"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		const FString Content = UKismetStringLibrary::JoinStringArray(Args);
		Log_RunLuaFunc(Content);
	})
);

static FAutoConsoleCommandWithWorldAndArgs RunLuaCmd(TEXT("Lua"), TEXT("执行Lua命令"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		FString LuaRunContent = UKismetStringLibrary::JoinStringArray(Args);
		UE_LOG(LogTemp, Log, TEXT("RunLua:%s"), *LuaRunContent);
		if (UnLua::GetState())
		{
			UnLua::RunChunk(UnLua::GetState(), TCHAR_TO_ANSI(*LuaRunContent));
		}
	})
   );
