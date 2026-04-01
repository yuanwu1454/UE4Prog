// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Global/MyGlobals.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	/** virtual function to allow custom GameInstances an opportunity to set up what it needs */
	virtual void Init() override;
	
	virtual void Shutdown() override;
public:
	UPROPERTY(Transient)
	UMyGlobals* MyGlobalsInstance;
	// 全局配置类
	UPROPERTY(Config)
	TSoftClassPtr<class UMyGlobals> MyGlobalsClass;

	void SetupGlobalsCfgObject();
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