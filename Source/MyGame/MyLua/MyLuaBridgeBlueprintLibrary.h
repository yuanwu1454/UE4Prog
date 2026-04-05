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
};
