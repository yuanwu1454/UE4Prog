// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCheatManager.h"
#include "MyPlayerController.h" // 你的玩家控制器头文件
#include "MyGameInstance.h"       // 你的游戏实例头文件

void UMyCheatManager::AddGold(int32 Amount)
{
	// 获取当前玩家控制器
	AMyPlayerController* PC = Cast<AMyPlayerController>(GetOuter());
	if (PC)
	{
		// 假设你的玩家状态类有AddGold方法
		// AMyPlayerState* PS = Cast<AMyPlayerState>(PC->PlayerState);
		// if (PS)
		// {
		// 	PS->AddGold(Amount);
		// 	PC->ClientMessage(FString::Printf(TEXT("作弊：添加了 %d 金币！"), Amount));
		// }
	}
}

void UMyCheatManager::UnlockAllLevels()
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		// GI->UnlockAllLevels();
		// 给玩家发送提示
		APlayerController* PC = Cast<APlayerController>(GetOuter());
		if (PC)
		{
			PC->ClientMessage("作弊：所有关卡已解锁！");
		}
	}
}

void UMyCheatManager::TravelToMap(FString MapName)
{
	APlayerController* PC = Cast<APlayerController>(GetOuter());
	if (PC && !MapName.IsEmpty())
	{
		// 跳转到指定地图
		// PC->ClientTravel(MapName, ETravelType::TRAVEL_LoadingScreen);
		PC->ClientMessage(FString::Printf(TEXT("作弊：正在跳转到地图 %s"), *MapName));
	}
	else
	{
		PC->ClientMessage("作弊：地图名称不能为空！");
	}
}

void UMyCheatManager::GodFly()
{
	// 组合内置作弊命令
	APlayerController* PC = Cast<APlayerController>(GetOuter());
	if (PC)
	{
		// 调用内置的god和fly命令
		PC->CheatManager->God();
		PC->CheatManager->Fly();
		PC->ClientMessage("作弊：已开启无敌+飞行模式！");
	}
}