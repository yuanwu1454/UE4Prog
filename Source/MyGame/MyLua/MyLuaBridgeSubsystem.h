#pragma once


//###Engine Include
#include "CoreMinimal.h"

#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"

//###Plugins Include
#include "UnLuaInterface.h"

// //###Project Include
// #include "PMDelegateWorldSubsystem.h"
// #include "PMLuaBridgeObject.h"
// #include "MyGameSubSystemBase.h"
#include "MyLua/MyLuaBridgeObject.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
//###Last Include
#include "MyLuaBridgeSubsystem.generated.h"
/**
 * 用于桥接Lua和C++层的数据中转
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLuaWindowActivatedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLuaWindowDeactivatedEvent);

UCLASS(config=Game)
class MYGAME_API UMyLuaBridgeSubsystem : public UMyGameInstanceSubsystem,public FTickableGameObject
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FOnLuaInit);

public:
	// 获取对象，如果在另一个SubsystemInit时获取有可能拉不到，所有Subsystem按字母顺序初始化
	UFUNCTION(BlueprintCallable)
	static UMyLuaBridgeSubsystem* Get(UObject* InWorldContextObject)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(InWorldContextObject);
		return UGameInstance::GetSubsystem<UMyLuaBridgeSubsystem>(GameInstance);
	}

	// void Tick(float DeltaTime) override;

	// 用于Lua系统自动获取World
	UFUNCTION()
	UWorld* GetLuaDefaultWorld();

	void Tick(float DeltaTime) override;

	bool IsLuaInit() const { return bLuaInit; }

	// UPMLuaBridgeObject* GetLuaBridgeObject() const { return LuaBridgeObject; }

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void OnStartUp() override;

	// 启动LuaSystem,Lua虚拟机在启动Module时就启动了，但业务代码需要在启动热更后加载。
	UFUNCTION()
	void OnStartupLuaSystem();

	// 初始化流程 先走OnLuaBridgeInit再走Startup
	UFUNCTION()
	void OnLuaBridgeInit();

	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override { return IsTemplate() ? ETickableTickType::Never : ETickableTickType::Conditional; }
	
	// 调用Lua出错时的处理
	void OnHandleLuaCallError(FString& ErrStr);
public:
	FOnLuaInit OnLuaInit;

private:

	// 标记lua是否初始化
	bool bLuaInit = false;

	UPROPERTY()
	UMyLuaBridgeObject* LuaBridgeObject ;
	
	UPROPERTY(Transient)
	UObject* LuaSysDefaultWorldObject ;
};

