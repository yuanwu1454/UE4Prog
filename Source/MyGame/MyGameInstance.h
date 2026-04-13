// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Global/MyGlobals.h"
#include "Engine/World.h" // 必须包含
#include "MyGameInstance.generated.h"

class UMyGameInstanceSubsystem;
class UGlobalStateMachine;
/**
 * 
 */
UCLASS()
class MYGAME_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	/** virtual function to allow custom GameInstances an opportunity to set up what it needs */
	virtual void Init() override;
	virtual void Shutdown() override;
	// virtual void StartGameInstance() override;
	
	/** Called when the game instance is started either normally or through PIE. */
	virtual void OnStart();
	
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Out = *GLog) override;
	
#if WITH_EDITOR
	virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params) override;
#endif
	
public:
	UPROPERTY(Transient)
	UMyGlobals* MyGlobalsInstance;
	// 全局配置类
	UPROPERTY(Config)
	TSoftClassPtr<class UMyGlobals> MyGlobalsClass;

	void SetupGlobalsCfgObject();
	void NtySubsystemsStartUp();
	void NtySubsystemsShutDown();
	void RegisterMyGameSubSystemBase(UMyGameInstanceSubsystem* Subsystem);
	UPROPERTY(Transient)
    TArray<UMyGameInstanceSubsystem*> CachedRegisterSubsystemArray;


	UPROPERTY()
	UGlobalStateMachine* GlobalStateMachine;
	UPROPERTY(Config)
	FSoftObjectPath GlobalStateMachineAsset;
	
	UGlobalStateMachine* GetGlobalStateMachine() const {return GlobalStateMachine;}


	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;
	bool Tick(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	void GotoLoginScene();
	UFUNCTION(BlueprintCallable)
	void GotoLobbyScene();

public:
	void SetOpenLobbyMsgTips(const FText& Msg);
	void ClearOpenLobbyTips();
	FText OpenLobbyMsgTips;

	void ReleaseCachedObject();
	
	bool bAllSubsystemFinishInit = false;

	virtual void OnHandleSystemError();
	virtual void OnShutdownAfterError();

	// 关卡切换开始（旧关卡即将销毁）
	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;

	// 外部可安全使用的回调签名
	void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IV);

	void OnLevelAddedToWorld( ULevel* Level, UWorld* World);
	void OnLevelRemovedFromWorld( ULevel* Level, UWorld* World);
};


// FExec 是一个核心接口类，用于实现「控制台命令执行」能力 —— 任何继承 FExec 的类，都可以响应 UE 控制台（Console）输入的自定义命令，这在调试、开发工具、玩家指令（如作弊码）等场景中非常常用。
// Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
// UWorld* InWorld：命令执行的「上下文环境」
// const TCHAR* Cmd：要执行的「命令内容」
// FOutputDevice& Ar：命令执行的「结果输出渠道」

// LocalPlayers
// 单人游戏：数组长度 = 1
// 分屏游戏：数组长度 = N
// 多人联机客户端：数组长度 = 1

// OnlineSession
// 就是在线子系统
// Steam、EOS、Xbox Live

// ReferencedObjects
// 是 UGameInstance 中用于手动管理对象生命周期的核心数组 —— 其核心作用是通过「强引用」阻止 UE 的垃圾回收（GC）系统销毁指定对象，在客户端和服务器的表现、用途、管理逻辑既有共性也有关键差异。
// RegisterReferencedObject, UnregisterReferencedObject

// NotifyPreClientTravelDelegates

// OnPawnControllerChangedDelegates
// 客户端和服务器的触发时机、触发条件、使用场景有显著差异 ——
// 核心原因是「Pawn-Controller 绑定」的逻辑在网络环境下分为「服务器权威操作」和「客户端本地同步」，且复制（Replication）机制会导致委托触发的特殊性。

// TimerManager
// 给到所持有的world使用

// 可以自定义引擎
// FEngineLoop::Init()
// [/Script/Engine.Engine]
// GameEngine=/Script/MyGame.MyGameEngine

// UObject 才能用配置文件替换；FSlateApplication 不是 UObject，所以不能。
// FSlateApplication 不行