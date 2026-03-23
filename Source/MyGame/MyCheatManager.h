// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "MyCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyCheatManager : public UCheatManager
{
	GENERATED_BODY()
public:
	// ========== 自定义作弊命令（核心） ==========
	// 规则：函数名就是控制台输入的命令名，参数会自动解析
    
	/** 刷指定数量的金币（控制台输入：AddGold 1000） */
	UFUNCTION(Exec)  // Exec标记：允许通过控制台执行
	void AddGold(int32 Amount = 100);

	/** 解锁所有关卡（控制台输入：UnlockAllLevels） */
	UFUNCTION(Exec)
	void UnlockAllLevels();

	/** 传送到指定场景（控制台输入：TravelToMap MyTestMap） */
	UFUNCTION(Exec)
	void TravelToMap(FString MapName);

	/** 给玩家添加无敌+飞行模式（组合作弊，控制台输入：GodFly） */
	UFUNCTION(Exec)
	void GodFly();
};
