// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyPlayerCharacter.h"
#include "MyGameMode.h"
#include "MyPlayerState.h"
#include "OnlineSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "Blueprint/UserWidget.h"
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
	// 绑定按键（如按ESC打开菜单）
	InputComponent->BindAction("OpenMenu", IE_Pressed, this, &AMyPlayerController::OnOpenMenu);
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

void AMyPlayerController::OnOpenMenu()
{
	// 加载UI蓝图（替换为你的UI路径）
	UClass* UIMenuClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/WBP_MainMenu.WBP_MainMenu_C"));
	if (UIMenuClass)
	{
		UUserWidget* MenuWidget = CreateWidget<UUserWidget>(this, UIMenuClass);
		OpenUIAndSetInputMode(MenuWidget);
	}
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

void AMyPlayerController::OpenUIAndSetInputMode(UUserWidget* TargetWidget)
{
	if (!TargetWidget) return;

	// 1. 创建输入模式对象（仅UI模式）
	FInputModeUIOnly InputModeUI;

	// （可选）指定焦点的UI控件（确保该控件能响应键盘/手柄导航）
	if (TargetWidget->GetRootWidget())
	{
		UWidget* Widget = TargetWidget->GetRootWidget();
		InputModeUI.SetWidgetToFocus(Widget->TakeWidget());
	}

	// 2. 设置输入模式（核心步骤）
	SetInputMode(InputModeUI);

	// 3. 显示鼠标光标（UI交互必备）
	bShowMouseCursor = true;

	// 4. 显示UI（可选，根据你的UI加载逻辑）
	TargetWidget->AddToViewport();
}

void AMyPlayerController::EnterGameOnlyInputMode()
{
	// 1. 创建“仅游戏输入”模式对象
	FInputModeGameOnly GameOnlyMode;
    
	// 可选：修改鼠标捕获事件的消耗规则（默认true）
	// GameOnlyMode.SetConsumeCaptureMouseDown(false);
    
	// 2. 应用输入模式（核心步骤）
	SetInputMode(GameOnlyMode);
    
	// 3. 可选：隐藏鼠标光标（游戏模式下通常不需要光标）
	bShowMouseCursor = false;


	// 方式1：通过创建UI的OwningPlayer获取（推荐，最安全）
	// if (APlayerController* PC = GetOwningPlayer())
	// {
	// 	if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(PC))
	// 	{
	// 		MyPC->RestoreGameInputMode();
	// 	}
	// }
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