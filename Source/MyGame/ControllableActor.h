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
};
