// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalInitializeState.h"

#include "MyGameInstance.h"

void UGlobalInitializeState::BeginState()
{
	bool bNeedFinishInitInOneFrame = false;


	const UStateEventData* Data = GetTriggerEventData();
	if (Data && Data->IsA(UGlobalInitStateEventData::StaticClass()))
	{
		const UGlobalInitStateEventData* EvtData = Cast<const UGlobalInitStateEventData>(Data);
		if (EvtData)
		{
			bNeedFinishInitInOneFrame = EvtData->bNeedQuickFinishAndWait;
		}
	}
	// 需要一帧内同步完成初始化
	if (bNeedFinishInitInOneFrame)
	{
		CleanPhaseResetData();
		InitPhaseLoadGameData();
		InitPhaseStartupGameinstanceSubsystem();
		InitPhaseStartDataCenterSubsystem();
		RunPostInitActions();
	}
	else
	{
		// 正常启动初始化
		Super::BeginState();
	}
}

void UGlobalInitializeState::EndState()
{
	Super::EndState();
}

void UGlobalInitializeState::ConfigEnterStateCoroutine(FCoroutine& PMCoroutine)
{
	Super::ConfigEnterStateCoroutine(PMCoroutine);
	
	Coroutine.ConfigWaitForFrame(1);
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([this]()
	{
		CleanPhaseResetData();
		// 启动子系统
		InitPhaseLoadGameData();
	}));
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([this]()
	{
		// 启动子系统
		InitPhaseStartupGameinstanceSubsystem();
	}));
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([this]()
	{
		InitPhaseStartDataCenterSubsystem();
		RunPostInitActions();
	}));
#if !UE_SERVER
	// 进Login
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([this]()
	{
		// 资源更新
		UGlobalLoginStateEventData* EventData = NewObject<UGlobalLoginStateEventData>();
		EventData->DebugMessage = "Init -> Login";
		if (const UGlobalInitStateEventData* TrigData = Cast<const UGlobalInitStateEventData>(GetTriggerEventData()))
		{
			EventData->PtrGameStartPageInst = TrigData->PtrGameStartPageInst;
		}
		this->GetStateMachine()->TransferState(static_cast<uint32>(EGlobalStateType::Login), EventData);
	}));
#endif
}

void UGlobalInitializeState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool UGlobalInitializeState::IsInitFromStartup() const
{	// 启动初始化时才触发子系统启动
	bool bInitForStartup = false;
	if (const UGlobalInitStateEventData* EvtData = Cast<const UGlobalInitStateEventData>(GetTriggerEventData()))
	{
		bInitForStartup = EvtData->bInitForStartup;
	}
	return bInitForStartup;
}

void UGlobalInitializeState::CleanPhaseResetData()
{
	if (UMyGameInstance* Ins = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
	{
		Ins->ReleaseCachedObject();
	}
	UE_LOG(LogTemp, Log, TEXT("ForceGarbageCollection with full purge!"));
	if (GEngine)
		GEngine->ForceGarbageCollection(true);
}

void UGlobalInitializeState::InitPhaseLoadGameData()
{
	if (UMyGameInstance* Ins = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
	{
		Ins->SetupGlobalsCfgObject();
	}
	UMyGlobals::Get().ReloadGameData();
}

void UGlobalInitializeState::InitPhaseStartupGameinstanceSubsystem()
{
	if (!IsInitFromStartup())
	{
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("InitPhaseStartupGameinstanceSubsystem"));
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		if (UMyGameInstance* Ins = Cast<UMyGameInstance>(GetWorld()->GetGameInstance()))
		{
			Ins->NtySubsystemsStartUp();
		}
	}
}

void UGlobalInitializeState::InitPhaseStartDataCenterSubsystem() const
{
}

void UGlobalInitializeState::RunPostInitActions()
{
}

void UGlobalInitializeState::CheckHighDPIUIRes()
{
}
