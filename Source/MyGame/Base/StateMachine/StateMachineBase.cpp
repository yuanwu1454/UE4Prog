// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineBase.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogStateMachine);

void UStateBase::BeginState()
{
	ConfigLoadMapCoroutine();
	ConfigEnterStateCoroutine(Coroutine);
	Coroutine.Run();
}

void UStateBase::EndState()
{
	Coroutine.Stop();
}

bool UStateBase::IsEnableTick()
{
	return bEnableTick;
}

void UStateBase::Tick(const float DeltaTime)
{
	Coroutine.Tick(DeltaTime);
}

void UStateBase::SetTriggerEventData(UStateEventData* EventData)
{
	TriggerEventData = EventData;
}

const UStateEventData* UStateBase::GetTriggerEventData() const
{
	return TriggerEventData;
}

IStateMachine* UStateBase::GetStateMachine()
{
	return WeakRefStateMachine.IsValid(false) ? WeakRefStateMachine.Get() : nullptr;
}

bool UStateBase::HandleEventData(UStateEventData* EventData)
{
	return false;
}

void UStateBase::ConfigEnterStateCoroutine(FCoroutine& PMCoroutine)
{
}

void UStateBase::ConfigLoadMapCoroutine()
{
	// DS不需要管地图加载（DS不走状态机）
#if !UE_SERVER	
	// 检查依赖的地图是否加载
	const FString MapAssetName = StateGameMap.GetAssetName();
	if (!MapAssetName.IsEmpty() && MapAssetName != GetWorld()->GetName())
	{
		// 需要加载地图
		Coroutine.ConfigNewAction(FCoroutineAction::CreateUObject(this, &UStateBase::LoadDependentMap));
		Coroutine.ConfigWaitAction(FCoroutineWaitAction::CreateLambda([this]()
		{
			const FString CurrentWorldName = this->GetWorld()->GetName();
			const FString PendingName = StateGameMap.GetAssetName();
			return CurrentWorldName.Equals(PendingName, ESearchCase::IgnoreCase);
		}));
	}
#endif
}

void UStateBase::LoadDependentMap()
{
	UGameplayStatics::OpenLevel(this->GetWorld(), FName(StateGameMap.GetLongPackageName()), true);
}

void UStateBase::BeginLoadingProcess()
{
}

void UStateBase::EndLoadingProcess()
{
}

void UStateMachineBase::Start()
{
	bRunning = true;
}

void UStateMachineBase::End()
{
	bRunning = false;
	if (CurrentState)
	{
		CurrentState->EndState();
		CurrentState = nullptr;
	}
}

void UStateMachineBase::Tick(float DeltaTime)
{
	if (bRunning)
	{
		if (CurrentState && CurrentState->IsEnableTick())
		{
			CurrentState->Tick(DeltaTime);
		}
	}
}

void UStateMachineBase::RegisterState(uint32 StateID, UClass* StateClass)
{
	if (StateMap.Contains(StateID))
	{
		UE_LOG(LogStateMachine, Warning, TEXT("StateID %d exist!"), StateID);
	}
	else
	{
		StateMap.Add(StateID, StateClass);
	}
}

void UStateMachineBase::ProcessEventData(UStateEventData* EventData)
{
	if (!bRunning)
	{
		UE_LOG(LogStateMachine, Warning, TEXT("UStateMachineBase::ProcessEventData StateMachine not start!"), CurrentState->GetStateID());
		return;
	}
	if (!CurrentState)
	{
		return;
	}
	if (!CurrentState->HandleEventData(EventData))
	{
		UE_LOG(LogStateMachine, Warning, TEXT("StateID %d Did Not Handle this EventData!"), CurrentState->GetStateID());
	}
}

void UStateMachineBase::ProcessTick(const float DeltaTime)
{
	if (!CurrentState)
	{
		return;
	}
	if (CurrentState->IsEnableTick())
	{
		CurrentState->Tick(DeltaTime);
	}
}

UStateBase* UStateMachineBase::GetCurrentState()
{
	return CurrentState;
}

void UStateMachineBase::TransferState(uint32 StateID, UStateEventData* EventData)
{
	if (CurrentState && CurrentState->GetStateID() == StateID)
	{
		UE_LOG(LogStateMachine, Log , TEXT("Try enter same state %d"), StateID);
		return;
	}
	UStateBase* NewState = nullptr;
	UClass** StateClassRef = StateMap.Find(StateID);
	if (StateClassRef)
	{
		 NewState = Cast<UStateBase>(DuplicateObject((*StateClassRef)->GetDefaultObject(), this));
	}
	if (NewState)
	{
		if (CurrentState)
		{
			CurrentState->EndState();
		}
		UE_LOG(LogStateMachine, Log, TEXT("TransferState %d"), StateID);
		CurrentState = NewState;
		CurrentState->WeakRefStateMachine = MakeWeakObjectPtr(this);
		CurrentState->SetStateID(StateID);
		CurrentState->SetTriggerEventData(EventData);
		if (OnGlobalStateTransfer.IsBound())
		{
			OnGlobalStateTransfer.Broadcast(StateID);
		}
		CurrentState->BeginState();
	}
}
