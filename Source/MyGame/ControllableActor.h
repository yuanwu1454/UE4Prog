// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControllableActor.generated.h"

UCLASS()
class MYGAME_API AControllableActor : public AActor
{
	GENERATED_BODY()
    
public:    
	// 构造函数
	AControllableActor();

	// 移动方法（基于世界方向）
	UFUNCTION(BlueprintCallable, Category = "Control")
	void MoveActor(const FVector& Direction, float Speed);

	// 旋转方法（基于欧拉角）
	UFUNCTION(BlueprintCallable, Category = "Control")
	void RotateActor(float YawDelta, float PitchDelta, float RollDelta, float RotationSpeed);

	// 获取根组件（方便Controller访问）
	UFUNCTION(BlueprintCallable, Category = "Components")
	USceneComponent* GetRootComp() const { return RootComponent; }

protected:
	// 游戏开始时调用
	virtual void BeginPlay() override;

private:
	// 根组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComponent;

	// 可视化网格体（用于显示Actor）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	// 移动速度（可在蓝图中调整）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MoveSpeed = 500.0f;

	// 旋转速度（可在蓝图中调整）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float RotationSpeed = 90.0f;
};
