// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyPlayerCharacter.h"
#include "MyGameMode.h"
#include "MyPlayerState.h"
#include "OnlineSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Log/MultiplayerLogHelper.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
// 绑定输入（PlayerController 初始化时自动调用）
void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 绑定按键 Q 到 OnTriggerQPressed（仅客户端生效）
	InputComponent->BindAction("TriggerQ", IE_Pressed, this, &AMyPlayerController::OnTriggerQPressed);
}

// 按键 Q 触发逻辑（客户端执行）
void AMyPlayerController::OnTriggerQPressed()
{
	// 仅客户端处理输入（服务器不处理按键）
	if (!IsLocalController()) return;

	// 获取当前操控的角色
	AMyPlayerCharacter* MyCharacter = GetControlledCharacter();
	if (!MyCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("未找到可操控的角色"));
		return;
	}

	MyCharacter->UpdateMoveSpeed(10.f);
	
	UE_LOG(LogTemp, Log, TEXT("客户端按键 Q"));
}

void AMyPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	
	// 记录新客户端登录，传入PlayerController作为PlayerContext
	MULTI_LOG(FString::Printf(TEXT("OnPossess：%s"), *this->GetName()), this, this);

	// 转换为自定义Character
	AMyPlayerCharacter* PlayerChar = Cast<AMyPlayerCharacter>(aPawn);
	if (!PlayerChar) return;

	PlayerChar->UpdateAttributes(44.f, 100);
}

void AMyPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	
	// 记录新客户端登录，传入PlayerController作为PlayerContext
	MULTI_LOG(FString::Printf(TEXT("OnRep_Pawn：%s"), *this->GetPlayerName()), this, this);
}

FString AMyPlayerController::GetPlayerName() const
{
	if (auto MyPlayerState = Cast<AMyPlayerState>(PlayerState))
	{
		return MyPlayerState->GetUniquePlayerName();
	}
	return GetFName().ToString();
}

// 安全获取操控的角色（加空检）
AMyPlayerCharacter* AMyPlayerController::GetControlledCharacter()
{
	return Cast<AMyPlayerCharacter>(GetPawn());
}