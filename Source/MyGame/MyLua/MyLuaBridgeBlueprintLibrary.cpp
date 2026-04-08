//###First Include
#include "MyLuaBridgeBlueprintLibrary.h"
#include "..\Base\Coroutine\Coroutine.h"


#include "MyLogging.h"
#include "LuaGlobal.h"
#include "MyGlobals.h"

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

UDataTable* UMyLuaBridgeBlueprintLibrary::GetConfigDataTable(FString Name)
{
	UScriptStruct* Struct = FindObject<UScriptStruct>(ANY_PACKAGE, *Name, true);
	if (Struct)
	{
		const UDataTable* DataTable = UMyGlobals::Get().GetTableAsset()->GetDataTable(Struct);
		UDataTable* DataTableNoneConst = const_cast<UDataTable*>(DataTable);
		return DataTableNoneConst;
	}
	return nullptr;
}


int64 UMyLuaBridgeBlueprintLibrary::GetTimeZoneOffset()
{
	return (FDateTime::Now() - FDateTime::UtcNow()).GetTotalSeconds();
}

FDateTime UMyLuaBridgeBlueprintLibrary::GetDateTimeFromUnix(int64 TimeStamp)
{
	return FDateTime::FromUnixTimestamp(TimeStamp);
}

FString UMyLuaBridgeBlueprintLibrary::GetLocalDateTimeStringFromUnix(int64 TimeStamp)
{
	return FDateTime::FromUnixTimestamp(TimeStamp + GetTimeZoneOffset()).ToString();
}

FTimespan UMyLuaBridgeBlueprintLibrary::GetTimeSpanFromUnix(int64 StartTime, int64 EndTime)
{
	return FTimespan::FromSeconds(EndTime - StartTime);
}

FText UMyLuaBridgeBlueprintLibrary::FormatAsDateTime(const FDateTime& InDateTime, const int32 DateStyle,
	const int32 TimeStyle)
{
	return FText::AsDateTime(InDateTime, static_cast<EDateTimeStyle::Type>(DateStyle),
		static_cast<EDateTimeStyle::Type>(TimeStyle), FText::GetInvariantTimeZone());
}

FText UMyLuaBridgeBlueprintLibrary::FormatAsDate(const FDateTime& InDateTime, const int32 DateStyle)
{
	return FText::AsDate(InDateTime, static_cast<EDateTimeStyle::Type>(DateStyle),
		FText::GetInvariantTimeZone());
}

FText UMyLuaBridgeBlueprintLibrary::FormatAsTime(const FDateTime& InDateTime, const int32 TimeStyle)
{
	return FText::AsTime(InDateTime, static_cast<EDateTimeStyle::Type>(TimeStyle),
		FText::GetInvariantTimeZone());
}

