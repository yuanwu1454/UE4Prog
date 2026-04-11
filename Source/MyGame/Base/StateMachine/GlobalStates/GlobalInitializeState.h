// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/GlobalStateMachine.h"
#include "GlobalInitializeState.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MYGAME_API UGlobalInitializeState : public UGlobalState
{
	GENERATED_BODY()
public:
	virtual void BeginState() override;
	virtual void EndState() override;
	virtual void ConfigEnterStateCoroutine(FCoroutine& PMCoroutine) override;
	virtual void Tick(const float DeltaTime) override;

private:
	bool IsInitFromStartup() const;
	
	// 重置全局数据
	UFUNCTION()
	void CleanPhaseResetData();
	
	// 加载数据
	UFUNCTION()
	void InitPhaseLoadGameData();
	
	// 启动子系统
	UFUNCTION()
	void InitPhaseStartupGameinstanceSubsystem();

	UFUNCTION()
	void InitPhaseStartDataCenterSubsystem() const;

	UFUNCTION()
	void RunPostInitActions();
	
	// Windows上是否启用了高清UI贴图
	UFUNCTION()
	void CheckHighDPIUIRes();
};
