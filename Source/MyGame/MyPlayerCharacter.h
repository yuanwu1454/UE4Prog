// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyPlayerCharacter();

	// ----------------------
	// 需同步的核心属性（标记Replicated）
	// ----------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float MoveSpeed = 600.0f; // 角色移动速度（必须同步）

	// ----------------------
	// 无需同步的临时属性（不标记Replicated，但默认仍会被UE遍历）
	// ----------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TempFPS = 60; // 本地帧率（仅本地用，同步无意义）

	// 网络同步核心：重写GetLifetimeReplicatedProps（声明要同步的属性）
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 重写Serialize：优化网络同步的字节流
	virtual void Serialize(FArchive& Ar) override;

	// 测试：修改属性并触发同步
	UFUNCTION(BlueprintCallable, Category="NetworkTest")
	void UpdateAttributes(float NewSpeed, int32 NewFPS);
	// 客户端调用的RPC
	UFUNCTION(Server, Reliable)
	void Server_UpdateAttributes(float NewSpeed, int32 NewFPS);


	// 测试：修改属性并触发同步
	UFUNCTION(BlueprintCallable, Category="NetworkTest")
	void UpdateMoveSpeed(float NewSpeed);
	// 客户端调用的RPC
	UFUNCTION(Server, Reliable)
	void Server_UpdateMoveSpeed(float NewSpeed);
	
	UFUNCTION()
	void OnRep_MoveSpeed();
};
