// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalStateMachine.h"

#include "MyGameInstance.h"
#include "SubSystem/GlobalDelegateManager.h"

FString GetGlobalStateName(EGlobalStateType StateType)
{
	FString StateName;
	switch (StateType)
	{
	case EGlobalStateType::Entry:
		StateName = TEXT("Entry");
		break;
	case EGlobalStateType::Init:
		StateName = TEXT("Init");
		break;
	case EGlobalStateType::ResUp:
		StateName = TEXT("ResUp");
		break;
	case EGlobalStateType::Login:
		StateName = TEXT("Login");
		break;
	case EGlobalStateType::Scenario:
		StateName = TEXT("Scenario");
		break;
	case EGlobalStateType::Lobby:
		StateName = TEXT("Lobby");
		break;
	case EGlobalStateType::Playing:
		StateName = TEXT("Playing");
		break;
	case EGlobalStateType::Transition:
		StateName = TEXT("Transition");
		break;
	default:
		StateName = TEXT("Unkonw");
		break;
	}
	return StateName;
}

void UGlobalState::TransferState(EGlobalStateType StateType, UStateEventData* EventData)
{
	GetStateMachine()->TransferState(static_cast<uint32>(StateType), EventData);
}

void UGlobalState::BeginState()
{
	Super::BeginState();
}

UGlobalStateMachine* UGlobalStateMachine::Get(const UObject* WorldObj)
{
	if (WorldObj)
	{
		if (UWorld* World = WorldObj->GetWorld())
		{
			if (UMyGameInstance* Inst = Cast<UMyGameInstance>(World->GetGameInstance()))
			{
				if (Inst->GetGlobalStateMachine())
				{
					return Inst->GetGlobalStateMachine();
				}
			}
		}
	}
	return nullptr;
}

void UGlobalStateMachine::Start()
{
	Super::Start();
	// 注册
	for(const FGlobalStateConfig& State : RegistedStateArray)
	{
		RegisterState(static_cast<uint32>(State.StateType), State.StateClass);
	}
	OnGlobalStateTransfer.AddWeakLambda(this, [this](uint32 StateID)
	{
		if (const UGlobalDelegateManager* DelegateManager = UGlobalDelegateManager::Get(this))
		{
			DelegateManager->OnGlobalStateTransfer.Broadcast(static_cast<EGlobalStateType>(StateID));
		}
	});
}

void UGlobalStateMachine::End()
{
	OnGlobalStateTransfer.Clear();
	Super::End();
}

void UGlobalStateMachine::TransferState(uint32 StateID, UStateEventData* EventData)
{
	uint32 CurStateID = CurrentState ? CurrentState->GetStateID() : -1;
	Super::TransferState(StateID, EventData);
	UE_LOG(LogStateMachine, Log, TEXT("StateTransfer %s=>%s"), *GetGlobalStateName(static_cast<EGlobalStateType>(CurStateID)),
		*GetGlobalStateName(static_cast<EGlobalStateType>(StateID)));
	EGlobalStateType  StateType = static_cast<EGlobalStateType>(CurStateID);
	if (StateType == EGlobalStateType::Entry)//启动游戏
	{
	}
	else if (StateType == EGlobalStateType::Init )//初始化
	{
	}
	else if(StateType == EGlobalStateType::Scenario)//剧情
	{
	}
	else if (StateType == EGlobalStateType::Lobby)//到达大厅
	{
	}
}

UGlobalState* UGlobalStateMachine::GetCurrentGlobalState()
{
	return Cast<UGlobalState>(GetCurrentState());
}

EGlobalStateType UGlobalStateMachine::GetCurrentGlobalStateType()
{
	const UGlobalState* State = GetCurrentGlobalState();
	if (!State)
	{
		return EGlobalStateType::None;
	}
	return static_cast<EGlobalStateType>(State->GetStateID());
}

FString UGlobalStateMachine::GetCurrentGlobalStateName()
{
	if (const UGlobalState* State = GetCurrentGlobalState())
	{
		return GetGlobalStateName(static_cast<EGlobalStateType>(State->GetStateID()));
	}
	return TEXT("--");
}

void UGlobalStateMachine::TransferGlobalState(EGlobalStateType StateType, UStateEventData* EventData)
{
	TransferState(static_cast<uint32>(StateType), EventData);
}

void UGlobalStateMachine::TransferGlobalPlayingState_Pvp(uint32 MapId, FString WorldUrl)
{
	UGlobalPlayingStateEventData* EvtData = NewObject<UGlobalPlayingStateEventData>();
	EvtData->MapId = MapId;
	EvtData->MapType = EPlayingMapType::PVP_Map;
	EvtData->MapUrl = WorldUrl;
	TransferGlobalState(EGlobalStateType::Playing, EvtData);
}

void UGlobalStateMachine::TransferGlobalPlayingState_Practice()
{
	UGlobalPlayingStateEventData* EvtData = NewObject<UGlobalPlayingStateEventData>();
	EvtData->MapType = EPlayingMapType::Practice_Map;
	TransferGlobalState(EGlobalStateType::Playing, EvtData);
}

void UGlobalStateMachine::TransferGlobalPlayingState_TeamGuide(uint64 UID, uint32 OptionalValue)
{
	UGlobalPlayingStateEventData* EvtData = NewObject<UGlobalPlayingStateEventData>();
	EvtData->MapType = EPlayingMapType::TeamGuide_Map;
	EvtData->UID = UID;
	EvtData->OptionalValue = OptionalValue;
	TransferGlobalState(EGlobalStateType::Playing, EvtData);
}

void UGlobalStateMachine::TransferGlobalPlayingState_Replay(uint32 MapId)
{
	UGlobalPlayingStateEventData* EvtData = NewObject<UGlobalPlayingStateEventData>();
	EvtData->MapId = MapId;
	EvtData->MapType = EPlayingMapType::Replay_Map;
	TransferGlobalState(EGlobalStateType::Playing, EvtData);
}

void UGlobalStateMachine::TransferGlobalPlayingState_UGCReplay(uint32 MapId, const FString& FileName, const FString& WorldUrl)
{
	UGlobalPlayingStateEventData* EvtData = NewObject<UGlobalPlayingStateEventData>();
	EvtData->MapId = MapId;
	EvtData->ReplayFileName = FileName;
	EvtData->MapUrl = WorldUrl;
	EvtData->MapType = EPlayingMapType::Replay_Map;
	if (GetCurrentGlobalState()->GetStateID() != static_cast<uint32>(EGlobalStateType::Playing))
	{
		TransferGlobalState(EGlobalStateType::Playing, EvtData);
	}
	// else if(auto CurrState = Cast<UGlobalPlayingState>(GetCurrentGlobalState()))
	// {
	// 	int StartIdx = EvtData->MapUrl.Find(TEXT("?UGCMapUrl="));
	// 	if (StartIdx != INDEX_NONE)
	// 	{
	// 		CurrState->SetTriggerEventData(EvtData);
	// 		CurrState->DownloadMapFile(EvtData);
	// 	}
	// }
}

void UGlobalStateMachine::TransferGlobalPlayingState_BombGuide(uint64 UID, uint32 OptionalValue)
{
	UGlobalPlayingStateEventData* EvtData = NewObject<UGlobalPlayingStateEventData>();
	EvtData->MapType = EPlayingMapType::BombGuide_Map;
	EvtData->UID = UID;
	EvtData->OptionalValue = OptionalValue;
	TransferGlobalState(EGlobalStateType::Playing, EvtData);
}

void UGlobalStateMachine::TransferGlobalPlayingState_UGC(FString WorldUrl)
{
	UGlobalPlayingStateEventData* EvtData = NewObject<UGlobalPlayingStateEventData>();
	EvtData->MapType = EPlayingMapType::UGC_Map;
	EvtData->MapUrl = WorldUrl;
	TransferGlobalState(EGlobalStateType::Playing, EvtData);
}

void UGlobalStateMachine::TransferGlobalTransitionState()
{
	TransferGlobalState(EGlobalStateType::Transition, nullptr);
}

void UGlobalStateMachine::TransferGlobalScenarioState()
{
	TransferGlobalState(EGlobalStateType::Scenario, nullptr);
}

void UGlobalStateMachine::EnsureCurrentGlobalState_Msg(EGlobalStateType StateType, FString EnsureMsg)
{
	if (GetCurrentGlobalStateType() != StateType)
	{
#if !WITH_EDITOR && !UE_BUILD_SHIPPING && !UE_SERVER
		FString InfoToLog = FString::Printf(TEXT("%s!CurState %s should be %s.Force Transfer!"), *EnsureMsg,
                                            			*GetCurrentGlobalStateName(), *GetGlobalStateName(StateType));
		Log_ScreenErr(InfoToLog);
#endif
		TransferGlobalState(StateType, nullptr);
	}
}