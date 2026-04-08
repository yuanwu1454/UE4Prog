#pragma once


//###Engine Include
#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"


//###Last Include
#include "MyLuaBridgeBlueprintLibrary.generated.h"

UCLASS()
class MYGAME_API UMyLuaBridgeBlueprintLibrary : public UBlueprintFunctionLibrary	, public IUnLuaInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Lua")
	static void Lua_UELOG(uint8 LuaLogLevel, FString Msg);

	UFUNCTION(BlueprintCallable, Category = "Lua")
	static void Test();

	
	/**************** 获取数据表 *********************/
	UFUNCTION(BlueprintCallable, Category = "Lua")
	static UDataTable* GetConfigDataTable(FString Name);
	
	/****************获得本地时区与UTC时差Unix时间偏移(秒)****************/
	UFUNCTION(BlueprintCallable, Category = "Lua")
	static int64 GetTimeZoneOffset();
	/****************根据Unix时间戳返回FDateTime时间结构****************/
	UFUNCTION(BlueprintCallable, Category = "Lua")
	static FDateTime GetDateTimeFromUnix(int64 TimeStamp);

	UFUNCTION(BlueprintCallable, Category = "Lua")
	static FString GetLocalDateTimeStringFromUnix(int64 TimeStamp);
	/****************根据Unix时间戳返回FTimespan时间差（EndTime-StartTime）****************/
	UFUNCTION(BlueprintCallable, Category = "Lua")
	static FTimespan GetTimeSpanFromUnix(int64 StartTime, int64 EndTime);
	/**格式化为日期时间文本*/
	UFUNCTION(BlueprintCallable, Category = "Lua")
	static FText FormatAsDateTime(const FDateTime& InDateTime, const int32 DateStyle, const int32 TimeStyle);
	/**格式化为日期文本*/
	UFUNCTION(BlueprintCallable, Category = "Lua")
	static FText FormatAsDate(const FDateTime& InDateTime, const int32 DateStyle);
	/**格式化为时间文本*/
	UFUNCTION(BlueprintCallable, Category = "Lua")
	static FText FormatAsTime(const FDateTime& InDateTime, const int32 TimeStyle);
};
