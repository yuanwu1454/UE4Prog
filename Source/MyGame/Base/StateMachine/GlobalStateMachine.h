// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachineBase.h"
#include "GlobalStateMachine.generated.h"


class UBaseUserWidget;
UENUM(BlueprintType)
enum class EGlobalStateType : uint8
{
	None,			// 未初始化状态
	Entry,			// 启动游戏，展示相关动画
	Init,			// 初始化，加载初始资源及配置
	ResUp,			// 资源更新
	Login,			// 登录
	Scenario,		// 剧情
	Lobby,			// 大厅
	Playing,		// 战斗
	Transition		// 中转，默认地图
};

UENUM(BlueprintType)
enum class EPlayingMapType : uint8
{
	PVP_Map,
	Practice_Map,
	TeamGuide_Map,
	Replay_Map,
	BombGuide_Map,
	UGC_Map
};


UENUM()
enum class EResUpReasonType : uint8
{
	Default,
	FirstResourceUpdate,	// 首包资源更新
	ResRepaire,				// 资源修复
};

extern FString GetGlobalStateName(EGlobalStateType StateType);

UCLASS()
class UGlobalState : public UStateBase
{
	GENERATED_BODY()
public:
	void TransferState(EGlobalStateType StateType, UStateEventData* EventData);
	virtual void BeginState() override;
};


USTRUCT(BlueprintType)
struct FGlobalStateConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	EGlobalStateType StateType;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStateBase> StateClass;
};

UCLASS()
class UGlobalEntryStateEventData : public UStateEventData
{
	GENERATED_BODY()
public:
	UClass* StudioPresentWidgetClass;	// 用于展示工作室的UI
};

UCLASS()
class UGlobalPlayingStateEventData : public UStateEventData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	uint32 MapId;
	UPROPERTY()
	EPlayingMapType MapType;
	UPROPERTY()
	FString MapUrl;
	UPROPERTY()
	FString ReplayFileName;
	UPROPERTY()
	uint64 UID;
	UPROPERTY()
	uint32 OptionalValue = 0;
};

UCLASS()
class UGlobalResUpStateEventData : public UStateEventData
{
	GENERATED_BODY()
public:
	EResUpReasonType ResUpReason = EResUpReasonType::Default;
	UBaseUserWidget* PtrGameStartPageInst = nullptr;		// 传递GameStartUp界面指针，到Login状态关闭
};


UCLASS()
class UGlobalInitStateEventData : public UStateEventData
{
	GENERATED_BODY()
public:
	bool bNeedQuickFinishAndWait = false;  // 非Startup启动，需要同一帧完成初始化.供其它系统使用.
	bool bInitForStartup = false;	      // 启动初始化，只有第一次进Init时才设置true.
	UBaseUserWidget* PtrGameStartPageInst = nullptr;		// 传递GameStartUp界面指针，到Login状态关闭
};

UCLASS()
class UGlobalLoginStateEventData : public UStateEventData
{
	GENERATED_BODY()
public:
	UBaseUserWidget* PtrGameStartPageInst = nullptr;		// 传递GameStartUp界面指针，到Login状态关闭
};

UCLASS()
class UGlobalLobbyStateEventData : public UStateEventData
{
	GENERATED_BODY()
public:
	// 加载streaming level
	bool bNeedLobbyStreamingLevel = false;
};


UCLASS(BlueprintType, Blueprintable)
class MYGAME_API UGlobalStateMachine : public UStateMachineBase
{
	GENERATED_BODY()
public:
	UFUNCTION()
	static UGlobalStateMachine* Get(const UObject* WorldObj);
	virtual void Start() override;
	virtual void End() override;
	// 状态迁移
	virtual void TransferState(uint32 StateID, UStateEventData* EventData) override;
	// 获取当前状态对象
	UFUNCTION()
	virtual UGlobalState* GetCurrentGlobalState();
	// 获取当前全局状态的类型
	UFUNCTION()
	virtual EGlobalStateType GetCurrentGlobalStateType();
	// 获取当前全局状态的名字
	UFUNCTION()
	virtual FString GetCurrentGlobalStateName();
	// 全局状态迁移
	UFUNCTION()
	void TransferGlobalState(EGlobalStateType StateType, UStateEventData* EventData);
	UFUNCTION()
	void TransferGlobalPlayingState_Pvp(uint32 MapId, FString WorldUrl);
	UFUNCTION()
	void TransferGlobalPlayingState_Practice();
	/*
	 * OptionalValue: 0-不需要自评；1-需要自评
	 */
	UFUNCTION()
	void TransferGlobalPlayingState_TeamGuide(uint64 UID, uint32 OptionalValue);
	UFUNCTION()
	void TransferGlobalPlayingState_Replay(uint32 MapId);
	UFUNCTION()
	void TransferGlobalPlayingState_UGCReplay(uint32 MapId, const FString& FileName, const FString& WorldUrl);
	UFUNCTION()
	void TransferGlobalPlayingState_BombGuide(uint64 UID, uint32 OptionalValue);
	UFUNCTION()
	void TransferGlobalPlayingState_UGC(FString WorldUrl);
	UFUNCTION()
	void TransferGlobalTransitionState();
	UFUNCTION()
	void TransferGlobalScenarioState();
	// 确保当前状态
	UFUNCTION()
	void EnsureCurrentGlobalState_Msg(EGlobalStateType StateType, FString EnsureMsg);

	//后续新的引导走这个接口切换地图
	// void TransferGlobalPlayingState_GuideTrain(uint64 UID, uint32 OptionalValue, ncmd_cs::GuideBattle GuideBattleType);
	
protected:
	// 配置注册的状态
	UPROPERTY(EditDefaultsOnly, Category = "States")
	TArray<FGlobalStateConfig> RegistedStateArray;
};



// 1. Blueprintable
// 核心作用：允许这个 C++ 类【被蓝图继承】
// 加了这个宏 → 你可以在 UE 编辑器里创建这个 C++ 类的蓝图子类
// 不加 → 无法创建蓝图子类，只能在 C++ 里用
// 适用场景
// 给需要蓝图扩展功能的类用，比如：
// 角色类 APlayerCharacter
// 游戏模式 AGameMode
// 自定义 Actor/Pawn

// 2. BlueprintType
// 核心作用：允许这个 C++ 类【作为变量类型在蓝图中使用】
// 加了这个宏 → 蓝图里能声明这个类的变量、传递参数
// 不加 → 蓝图里找不到这个类型，无法作为变量使用
// 适用场景
// 给纯数据类、结构体替代类、工具类用，比如：
// 自定义数据载体 UUserData
// 配置类 UConfigSettings
// 不需要被蓝图继承，只需要当变量用的类
