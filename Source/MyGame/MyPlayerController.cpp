// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyPlayerCharacter.h"
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
	AMyPlayerCharacter* Character = GetControlledCharacter();
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("未找到可操控的角色"));
		return;
	}

	Character->UpdateMoveSpeed(10.f);
	
	UE_LOG(LogTemp, Log, TEXT("客户端按键 Q"));
}

// 安全获取操控的角色（加空检）
AMyPlayerCharacter* AMyPlayerController::GetControlledCharacter()
{
	return Cast<AMyPlayerCharacter>(GetPawn());
}