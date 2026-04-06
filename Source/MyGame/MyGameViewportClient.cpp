// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameViewportClient.h"

#include "BasePlayerController.h"
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"

UMyGameViewportClient::UMyGameViewportClient()
{
	UE_LOG(LogTemp, Log, TEXT("UMyGameViewportClient Construct"));
}	

UMyGameViewportClient::UMyGameViewportClient(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Log, TEXT("UMyGameViewportClient ObjectInitializer Construct"));
}

void UMyGameViewportClient::ReceivedFocus(FViewport* InViewport)
{
	Super::ReceivedFocus(InViewport);
	UE_LOG(LogTemp, Log, TEXT("ReceivedFocus"));

	// 游戏窗口一激活 → 立刻隐藏鼠标、设置输入模式
	// AMyPlayerController* PC = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	// if (PC)
	// {
	// 	PC->EnterGameOnlyInputMode();
	// }
}

void UMyGameViewportClient::LostFocus(FViewport* InViewport)
{
	Super::LostFocus(InViewport);

	UE_LOG(LogTemp, Log, TEXT("LostFocus"));
}
