//###First Include
#include "MyLuaBridgeBlueprintLibrary.h"
#include "Coroutine/MyCoroutine.h"


#include "MyLogging.h"
#include "LuaGlobal.h"

void UMyLuaBridgeBlueprintLibrary::Lua_UELOG(uint8 LuaLogLevel, FString Msg)
{
	switch (LuaLogLevel)
	{
	case 1:
		UE_LOG(LogLua, Fatal, TEXT("%s "), *Msg);
		Log_ScreenErr(Msg);
		break;
	case 2:
		UE_LOG(LogLua, Error, TEXT("%s "), *Msg);
		Log_ScreenErr(Msg);
		break;
	case 3:
		UE_LOG(LogLua, Warning, TEXT("%s "), *Msg);
		break;
	case 4:
		UE_LOG(LogLua, Display, TEXT("%s "), *Msg);
		break;
	case 5:
		UE_LOG(LogLua, Log, TEXT("%s "), *Msg);
		break;
	case 6:
		UE_LOG(LogLua, Verbose, TEXT("%s "), *Msg);
		break;
	case 7:
		UE_LOG(LogLua, VeryVerbose, TEXT("%s "), *Msg);
		break;
	case 8:
		UE_LOG(LogLua, All, TEXT("%s "), *Msg);
		break;
	default:
		break;
	}
}

void UMyLuaBridgeBlueprintLibrary::Test()
{
	// 无参
	bool bOK1 = LuaGlobal::CallVoidLua("ShowCommonTip");;
	if (bOK1)
	{
		UE_LOG(LogTemp, Log, TEXT("ShowCommonTip called success"));
	}
	
	// 无参
	int32 number = LuaGlobal::CallLua<int32>("GetNumber");;
	UE_LOG(LogTemp, Log, TEXT("GetNumber called success %d"), number);
	
	
	int32 addValue = LuaGlobal::CallLua<int32>("AddNumber", 1, 2);
	UE_LOG(LogTemp, Log, TEXT("GetNumber called success %d"), addValue);
	
}
