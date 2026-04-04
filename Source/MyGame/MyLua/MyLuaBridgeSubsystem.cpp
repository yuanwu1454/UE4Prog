//###First Include
#include "MyLuaBridgeSubsystem.h"

//###Engine Include
#include "CoreMinimal.h"
#include "IPlatformFilePak.h"

//###Plugins Include
#include "UnLua/Public/UnLua.h"
#include "UnLuaDelegates.h"
#include "Internationalization/Regex.h"
#include "HAL/IConsoleManager.h"

static void LoadEntryPoint(lua_State* L)
{
	if (UnLua::Call(UnLua::GetState(), "require", "EntryPoint").IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[UMyLuaBridgeSubsystem.LoadEntryPoint] EntryPoint Loaded Success"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[UMyLuaBridgeSubsystem.LoadEntryPoint] EntryPoint Loaded failed"));
	}
}


UWorld* UMyLuaBridgeSubsystem::GetLuaDefaultWorld()
{
	if (IsValid(LuaSysDefaultWorldObject))
	{
		return LuaSysDefaultWorldObject->GetWorld();
	}
	else
	{
		return GetWorld();
	}
}


void UMyLuaBridgeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

	lua_State* L = UnLua::GetState();
    if (!L)
    {
    	UnLua::Startup();
    	UE_LOG(LogTemp, Log, TEXT("[UMyLuaBridgeSubsystem::OnStartupLuaSystem] call StartUp"));
    	L = UnLua::GetState();
    }
	
    FUnLuaDelegates::CallLuaError.Unbind();
    FUnLuaDelegates::CallLuaError.BindUObject(this, &UMyLuaBridgeSubsystem::OnHandleLuaCallError);
    // Step.1 加载入口
    LoadEntryPoint(L);
    // 注册相关全局变量
    UnLua::PushUObject(L, this);
    lua_setglobal(L, "g_LuaBridgeSubsystem");

    // Step.3 最后创建BridgeObject及lUA对象,最后一步
    LuaBridgeObject = NewObject<UMyLuaBridgeObject>();
    
    bLuaInit = true;

    UE_LOG(LogTemp, Log, TEXT("[UMyLuaBridgeSubsystem::OnStartupLuaSystem] OnStartupLuaSystem"));

    OnLuaInit.Broadcast();
	// UE_LOG(LogTemp, Log, TEXT("[UMyLuaBridgeSubsystem::Initialize] Initialize"));
	// const auto GameInstance = Cast<UPMGameInstance>(GetGameInstance());
	// if (!GameInstance || GameInstance->GetWorldContext()->RunAsDedicated || IsRunningDedicatedServer())
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("[UMyLuaBridgeSubsystem::Initialize] Running As DS without lua system"));
	// 	return;
	// }
#if UE_BUILD_SHIPPING
	// 如果是Lua文件，并且是Shipping构建，则不允许从外部加载，必须是cooked pak加载。
	if (!FPakPlatformFile::GetFilenameSecurityDelegate().IsBound())
	{
		FPakPlatformFile::GetFilenameSecurityDelegate().BindLambda([](const TCHAR* FileName)
		{
			const FString StrFileName = FileName;
			if (StrFileName.EndsWith(".lua") || StrFileName.EndsWith(".luac"))
			{
				return false;
			}
			return true;
		});
	}
#endif
	// 依赖网略除始化
	// Collection.InitializeDependency(UPMNetworkSubSystem::StaticClass());
	// Collection.InitializeDependency(UPMGlobalDelegateManager::StaticClass());
	// Collection.InitializeDependency(UPMVoiceManager::StaticClass());
	Super::Initialize(Collection);

	/*
	if (GetWorld()->WorldType == EWorldType::PIE)
	{
		// 主动初始化lua环境
		OnLuaBridgeInit();
	}
	else
	{
		FTimerManager& TimerManager = GameInstance->GetTimerManager();
		TimerManager.SetTimerForNextTick(this, &UMyLuaBridgeSubsystem::OnLuaBridgeInit);
	}
	*/
}

void UMyLuaBridgeSubsystem::OnLuaBridgeInit()
{
	/*
	auto DelegateManager = UPMGlobalDelegateManager::Get(this);
	DelegateManager->OnStartupLuaSystem.AddDynamic(this, &UMyLuaBridgeSubsystem::OnStartupLuaSystem);
	// @todo:jason.yu 这里应该在合适时机调用，在启动资源热更后
	DelegateManager->OnStartupLuaSystem.Broadcast();
	*/
}

void UMyLuaBridgeSubsystem::OnStartupLuaSystem()
{

}


// TStatId UMyLuaBridgeSubsystem::GetStatId() const
// {
// 	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyLuaBridgeSubsystem, STATGROUP_Tickables);
// }

void UMyLuaBridgeSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UMyLuaBridgeSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if(!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	// DS不需要
	if(const UWorld* World = Outer->GetWorld())
	{
		return World->GetNetMode() != ENetMode::NM_DedicatedServer;
	}
	return false;
}

void UMyLuaBridgeSubsystem::OnStartUp()
{
	Super::OnStartUp();
	if (!bLuaInit)
		OnStartupLuaSystem();
}

void UMyLuaBridgeSubsystem::Tick(float DeltaTime)
{
	if (bLuaInit)
	{
		if (LuaBridgeObject)
		{
			LuaBridgeObject->CustomTick(DeltaTime);
		}
	}
}


TStatId UMyLuaBridgeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMyLuaBridgeSubsystem, STATGROUP_Tickables);
}

void UMyLuaBridgeSubsystem::OnHandleLuaCallError(FString& ErrStr)
{
	// (\"[\w]:.*\/PM\/)
	// https://regex101.com
	FRegexPattern MatchPatter(FString(TEXT("(\"[\\w]:.*/PM/)")));
	FRegexMatcher Matcher(MatchPatter, ErrStr);
	if (Matcher.FindNext())
	{
		ErrStr = ErrStr.Replace(*Matcher.GetCaptureGroup(0), TEXT("\""));
	}

	// Log_ScreenErr(ErrStr);
	// @TODO:jason.yu 上扡到bugly或crashsight的自定义错误
}