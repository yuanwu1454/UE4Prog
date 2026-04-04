//###First Include
#include "MyLuaBridgeBlueprintLibrary.h"


#include "MyLogging.h"

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