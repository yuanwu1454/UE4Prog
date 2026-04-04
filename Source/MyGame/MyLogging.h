#pragma once

#include "Logging/LogMacros.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"

//###Project Include

#if UE_SERVER
#define DECLARE_LOG_CATEGORY_QUICK(CategoryName, DefaultVerbosity, CompileTimeVerbosity) DECLARE_LOG_CATEGORY_EXTERN(CategoryName, Warning, All);
#else
#define DECLARE_LOG_CATEGORY_QUICK(CategoryName, DefaultVerbosity, CompileTimeVerbosity) DECLARE_LOG_CATEGORY_EXTERN(CategoryName, DefaultVerbosity, CompileTimeVerbosity);
#endif

#if UE_BUILD_SHIPPING
#define DECLARE_LOG_CATEGORY_SHIPPING_MIN_WARN(CategoryName, DefaultVerbosity, CompileTimeVerbosity) DECLARE_LOG_CATEGORY_EXTERN(CategoryName, Warning, CompileTimeVerbosity);
#else
#define DECLARE_LOG_CATEGORY_SHIPPING_MIN_WARN(CategoryName, DefaultVerbosity, CompileTimeVerbosity) DECLARE_LOG_CATEGORY_EXTERN(CategoryName, DefaultVerbosity, CompileTimeVerbosity);
#endif


DECLARE_LOG_CATEGORY_EXTERN(LogPM, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMGameMode, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMGameState, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMPlayerController, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMAkAudio, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogPMWeapon, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogCMachine, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogAIMachine, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogWpnMachine, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogAtkSwitcher, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogHoldSwitcher, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogFireSwitch, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogFireSwitcher, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogCyAbility, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogPMAI, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogPMCsv, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMDataCenter, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMOnline, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMUI, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMUITick, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMUIJoystick, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMUIAutoFire, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMUISceneMap, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMLoading, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMCharacterMovement, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMCamera, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMPlayerState, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPMGC, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogLDSdk, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogMSDKSdk, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogNotice, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogPufferSdk, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogDolphinSdk, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCrashSightSdkSubSystem, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogAppsFlyerSdk, Log, All);
// 新手引导
DECLARE_LOG_CATEGORY_EXTERN(LogCyGuide, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogCyNoviceGuide, Log, All);

// 任务
DECLARE_LOG_CATEGORY_SHIPPING_MIN_WARN(LogCyTaskCore, Log, All);
DECLARE_LOG_CATEGORY_SHIPPING_MIN_WARN(LogCyGameTask, Log, All);
DECLARE_LOG_CATEGORY_SHIPPING_MIN_WARN(LogCyGuideTask, Log, All);
DECLARE_LOG_CATEGORY_SHIPPING_MIN_WARN(LogCyAchievement, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogDamageInfo, Log, All);

DECLARE_LOG_CATEGORY_QUICK(LogPMCharacter, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogDataTracker, Log, All);
DECLARE_LOG_CATEGORY_QUICK(LogBattleReport, Log, All);
DECLARE_LOG_CATEGORY_QUICK(LogInGameVoice, Log, All);

// 剧情
DECLARE_LOG_CATEGORY_EXTERN(Log_Sequence, Log, All);

// AVG事件
DECLARE_LOG_CATEGORY_EXTERN(Log_AVGEvent, Log, All);

// 作弊
DECLARE_LOG_CATEGORY_EXTERN(LogCheat, Log, All);

// Lua
DECLARE_LOG_CATEGORY_EXTERN(LogLua, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(Log_TssSdk, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(Log_SafeSDK, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(Log_Midas, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(Log_InternationPay, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(Log_LoginOfficial, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(Log_GVoiceNotify, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogSteamSdkSub, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogTLogHttp, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogWeGame, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogGCloudSdk, Log, All);

// 资源更新
DECLARE_LOG_CATEGORY_EXTERN(LogGameResUpdate, Log, All);

// 反作弊
DECLARE_LOG_CATEGORY_EXTERN(LogACE, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogDsNetwork, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogWonderfulMomentEvent, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogOnlineSdk, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogPandoraSdk, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogEsBrdSdk, Log, All);

// 统计数据
DECLARE_LOG_CATEGORY_EXTERN(LogCyStatAttributeSet, Log, All);

DECLARE_LOG_CATEGORY_EXTERN(LogAIHelp, Log, All);
//网络加速
DECLARE_LOG_CATEGORY_EXTERN(LogGRomeLink, Log, All);

//Roguelike
DECLARE_LOG_CATEGORY_QUICK(LogCyRoguelike, Log, All);

#define PM_LOG(CategoryName ,Format, ...) Game_LOG(CategoryName ,Format, ##__VA_ARGS__)
#define PM_LOG_WARN(CategoryName ,Format, ...) Game_LOG_WARN(CategoryName ,Format, ##__VA_ARGS__)
#define PM_LOG_ERROR(CategoryName ,Format, ...) Game_LOG_ERROR(CategoryName ,Format, ##__VA_ARGS__)

#define PM_PrintScreenGreen(CategoryName, Verbosity, TimeToDisplay, Format, ...) Game_PrintScreenGreen(CategoryName ,Verbosity, Format, ##__VA_ARGS__)
#define PM_PrintScreenRed(CategoryName, Verbosity, TimeToDisplay, Format, ...) Game_PrintScreenRed(CategoryName ,Verbosity, Format, ##__VA_ARGS__)


#if PLATFORM_WINDOWS
#if _MSC_VER
#define FUNC_NAME_C TEXT(__FUNCTION__)
#elif __clang__
#define FUNC_NAME_C __FUNCTION__
#else // FIXME - GCC? 
#define FUNC_NAME_C TEXT(__func__)
#endif
#else
#define FUNC_NAME_C TEXT("")
#endif


#define LINE_NO   __LINE__

#if NO_LOGGING

#define Game_LOG_Verbosity(CategoryName, Verbosity, Object, Format, ...) {}

#else

#define Game_LOG_Verbosity(CategoryName, Verbosity, Object, Format, ...) \
{ \
	static_assert((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Verbosity must be constant and in range."); \
	CA_CONSTANT_IF((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) <= ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY && (ELogVerbosity::Warning & ELogVerbosity::VerbosityMask) <= FLogCategory##CategoryName::CompileTimeVerbosity) \
	{ \
		UE_LOG_EXPAND_IS_FATAL(Verbosity, PREPROCESSOR_NOTHING, if (!CategoryName.IsSuppressed(ELogVerbosity::Verbosity))) \
		{ \
			FString TempMsgContent = FUNC_NAME_C; \
			TempMsgContent.Append("() : ");\
			TempMsgContent.Append(FString::Printf(TEXT(Format), ##__VA_ARGS__));\
			if(Object){ \
				TempMsgContent.Append(" "); \
				AppendObjectLogString(TempMsgContent, Object);\
			} \
			UE_LOG(CategoryName, Verbosity, TEXT("%s"), *TempMsgContent);\
		} \
	} \
}
#endif

#define Game_LOG(CategoryName, Format, ...) \
{ \
	Game_LOG_Verbosity(CategoryName, Log, this, Format, ##__VA_ARGS__) \
}

#define Game_LOG_OBJ(CategoryName, Object, Format, ...) \
{ \
	Game_LOG_Verbosity(CategoryName, Log, Object, Format, ##__VA_ARGS__) \
}

#define Game_LOG_OBJ_Struct(CategoryName, Object, Format, ...) \
{ \
	Game_LOG_Verbosity(CategoryName, Log, Object, Format, ##__VA_ARGS__) \
}

#define Game_LOG_WARN(CategoryName, Format, ...) \
{ \
	Game_LOG_Verbosity(CategoryName, Warning, this, Format, ##__VA_ARGS__) \
}

#define Game_LOG_ERROR(CategoryName, Format, ...) \
{ \
	Game_LOG_Verbosity(CategoryName, Error, this, Format, ##__VA_ARGS__) \
}

#define Game_PrintScreenGreen(CategoryName, Verbosity, TimeToDisplay, Format, ...) \
{ \
}

#define Game_PrintScreenRed(CategoryName, Verbosity, TimeToDisplay, Format, ...) \
{ \
}

#define LOG_SCREEN_ERROR(Format, ...) \
{ \
	const FString& ScreenErr = FString::Printf(TEXT(Format), ##__VA_ARGS__);\
	Log_ScreenErr(ScreenErr);\
}

void Log_ScreenErr(const FString& LogString);
void Log_ScreenErr_Clear();
void Log_ScreenErr_Toggle();

void AppendObjectLogString(FString& Result, const UActorComponent* InComponent);
void AppendObjectLogString(FString& Result, const AActor* InActor);
void AppendObjectLogString(FString& Result, const class USubsystem* InTarget);
void AppendObjectLogString(FString& Result, const class UUserWidget* InWidget);
void AppendObjectLogString(FString& Result, const UGameInstance* InTarget);
void AppendObjectLogString(FString& Result, const UWorld* InTarget);


