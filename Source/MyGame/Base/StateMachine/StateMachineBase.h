// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Coroutine/Coroutine.h"
#include "StateMachineBase.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogStateMachine, Log, All);


UCLASS()
class UStateEventData: public UObject
{
	GENERATED_BODY()
public:
	// 调试用
	UPROPERTY()
	FString DebugMessage;
};


class IStateMachine
{
protected:
	virtual ~IStateMachine() = default;
public:
	virtual void Start() = 0;
	virtual void End() = 0;
	virtual void Tick(float DeltaTime) = 0;
	virtual void RegisterState(uint32 StateID, UClass* StateClass) = 0;
	virtual void ProcessEventData(UStateEventData* EventData) = 0;
	virtual void ProcessTick(const float DeltaTime) = 0;
	virtual void TransferState(uint32 StateID, UStateEventData* EventData) = 0;
};

class IStateMachineState
{
protected:
	virtual ~IStateMachineState() = default;
public:
	virtual void BeginState() = 0;
	virtual void EndState() = 0;
	virtual bool IsEnableTick() = 0;
	virtual void Tick(const float DeltaTime) = 0;
	virtual void SetTriggerEventData(UStateEventData* EventData) = 0;
	virtual const UStateEventData* GetTriggerEventData() const = 0;
	virtual IStateMachine* GetStateMachine() = 0;
	virtual bool HandleEventData(UStateEventData* EventData) = 0;
};

class UStateBase;

/**
 * 
 */
UCLASS(BlueprintType)
class MYGAME_API UStateMachineBase : public UObject, public IStateMachine
{
	GENERATED_BODY()
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnGlobalStateTransfer, uint32);
	virtual void Start() override;
	virtual void End() override;
	virtual void Tick(float DeltaTime) override;
	virtual void RegisterState(uint32 StateID, UClass* StateClass) override;
	virtual void ProcessEventData(UStateEventData* EventData) override;
	virtual void ProcessTick(const float DeltaTime) override;
	virtual UStateBase* GetCurrentState();

	FOnGlobalStateTransfer OnGlobalStateTransfer;
protected:
	virtual void TransferState(uint32 StateID, UStateEventData* EventData) override;
	UPROPERTY()
	UStateBase* CurrentState;
private:
	UPROPERTY(Transient)
	TMap<uint32, UClass*> StateMap;

	bool bRunning = false;
};


UCLASS(BlueprintType, Blueprintable)
class UStateBase : public UObject, public IStateMachineState
{
	GENERATED_BODY()
public:
	virtual void BeginState() override;
	virtual void EndState() override;
	virtual bool IsEnableTick() override;
	virtual void Tick(const float DeltaTime) override;
	virtual void SetTriggerEventData(UStateEventData* EventData) override;
	virtual const UStateEventData* GetTriggerEventData() const override;
	virtual IStateMachine* GetStateMachine() override;
	virtual bool HandleEventData(UStateEventData* EventData) override;
protected:
	/**
	 * @brief 配置进度状态的过程
	 * @param PMCoroutine 协程对象
	 */
	virtual void ConfigEnterStateCoroutine(FCoroutine& PMCoroutine);
	/**
	 * @brief 用于配置加载地图的协程过程
	 */
	virtual void ConfigLoadMapCoroutine();
	UFUNCTION()
	void LoadDependentMap();
	UFUNCTION()
	void BeginLoadingProcess();
	UFUNCTION()
	void EndLoadingProcess();
public:
	uint32 GetStateID() const
	{
		return StateID;
	}

	void SetStateID(const uint32 StateIDParam)
	{
		this->StateID = StateIDParam;
	}
	UPROPERTY()
	TWeakObjectPtr<UStateMachineBase> WeakRefStateMachine;
protected:
	UPROPERTY()
	uint32 StateID;
	UPROPERTY(Transient)
	UStateEventData* TriggerEventData;
	UPROPERTY(EditDefaultsOnly)
	bool bEnableTick = true;
	UPROPERTY(Transient)
	FCoroutine Coroutine;
	// the map this state dependent
	UPROPERTY(EditDefaultsOnly, meta=(AllowedClasses="World"))
	FSoftObjectPath StateGameMap;
};
