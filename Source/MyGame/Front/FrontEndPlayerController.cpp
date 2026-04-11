// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontEndPlayerController.h"

#include "MyWidgetInteractionComponent.h"
#include "MyLua/MyLuaBridgeSubsystem.h"
#include "StateMachine/GlobalStateMachine.h"


AFrontEndPlayerController::AFrontEndPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 不想自动管理照相机
	bAutoManageActiveCameraTarget = false;

	MyWidgetInteractionComp = CreateDefaultSubobject<UMyWidgetInteractionComponent>(TEXT("WidgetInteractionComp"));
	if (MyWidgetInteractionComp)
	{
		MyWidgetInteractionComp->SetupAttachment(RootComponent);
	}
}

void AFrontEndPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);
	
	if(GetWorld())
	{
		UGameViewportClient* GameViewportClient = GetWorld()->GetGameViewport();
		if(GameViewportClient && GameViewportClient->Viewport)
		{
			if(GameViewportClient->Viewport->GetWindowMode() != EWindowMode::Windowed)
			{
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
			}
		}
	}
	
	SetEnable3DUIInteraction(false);
	
	// 非新手关的情况下，需要有机会验证当前的状态是否为Playing
	if (UGlobalStateMachine* SM = UGlobalStateMachine::Get(this))
	{
		SM->EnsureCurrentGlobalState_Msg(EGlobalStateType::Lobby, TEXT("PlayingConroller Check State."));
	}
}

void AFrontEndPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AFrontEndPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AFrontEndPlayerController::NotifyLoadedWorld(FName WorldPackageName, bool bFinalDest)
{
	Super::NotifyLoadedWorld(WorldPackageName, bFinalDest);
}

void AFrontEndPlayerController::BuildInputStack(TArray<UInputComponent*>& InputStack)
{
	Super::BuildInputStack(InputStack);
}

void AFrontEndPlayerController::SetEnableInput(bool bEnable)
{
}

void AFrontEndPlayerController::SetEnable3DUIInteraction(bool bEnable)
{
}
