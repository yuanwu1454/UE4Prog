// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "SubSystem/MyDynamicEngineSubsystem.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
#include "UI/SlateEventsHelper.h"
#include "Test/MySlateWidget.h"
#include "UnLua.h"

void UMyGameInstance::Init()
{
	Super::Init();

	FSlateEventsHelper::Get().Initialize();
	
	NtySubsystemsStartUp();

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
	NtySubsystemsShutDown();
	CachedRegistedSubsystemArray.Empty();
	
	Super::Shutdown();

	SMySlateWidget::GMySlateWidgetRoot.Reset();
	FSlateEventsHelper::Get().Shutdown();
}

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
	for (auto RegistedSubsystem : CachedRegistedSubsystemArray)
	{
		RegistedSubsystem->OnStartUp();
	}
}

void UMyGameInstance::NtySubsystemsShutDown()
{
	for (auto RegistedSubsystem : CachedRegistedSubsystemArray)
	{
		RegistedSubsystem->OnShutDown();
	}
}

void UMyGameInstance::RegisterMyGameSubSystemBase(UMyGameInstanceSubsystem* Subsystem)
{
	if (Subsystem)
	{
		UE_LOG(LogTemp, Log, TEXT("PMGameinstanceSubsystem %s Regist!"), *Subsystem->GetName());
		CachedRegistedSubsystemArray.Add(Subsystem);
	}
}


void UMyGameInstance::TestSimpleLuaCall()
{
	// 创建 Lua 状态
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	// 执行一句最简单的 Lua 代码
	int ret = luaL_dostring(L, "print('Hello from Lua!')");

	if (ret == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("✅ Lua 运行成功！"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Lua 运行失败！"));
	}
	luaL_newmetatable(L, "zxvxzv");
	// 关闭
	lua_close(L);
}
