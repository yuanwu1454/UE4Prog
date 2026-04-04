#pragma once


//###Engine Include
#include "CoreMinimal.h"


//###Plugins Include
#include "UnLuaInterface.h"

//###Last Include
#include "MyLuaBridgeObject.generated.h"

UCLASS(config = Game, defaultconfig)
class MYGAME_API UMyLuaBridgeObject : public UObject
	,public IUnLuaInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UMyLuaBridgeObject();

	void CustomTick(float DeltaTime);

	// TickLua
	UFUNCTION(BlueprintImplementableEvent)
	void TickLuaBridge(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)	
	void OnAutoRegist();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDelegates();

	UPROPERTY(config)
	FString LuaBridgeScript;
	
	virtual FString GetModuleName_Implementation() const override
	{
		return LuaBridgeScript;
	}
private:
	void AutoBindDelegates();
	bool bHaveAutoBind = false;
};

