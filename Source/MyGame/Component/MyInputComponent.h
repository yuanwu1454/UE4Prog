// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputComponent.h"
#include "MyInputComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UMyInputComponent : public UInputComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMyInputComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};

// TArray<FInputKeyBinding> KeyBindings;         // 直接绑按键
// TArray<FInputTouchBinding> TouchBindings;     // 触摸
// TArray<FInputAxisBinding> AxisBindings;       // 轴（W/S/A/D/鼠标）
// TArray<FInputAxisKeyBinding> AxisKeyBindings; // 轴按键
// TArray<FInputVectorAxisBinding> VectorAxisBindings; // 3D 轴
// TArray<FInputGestureBinding> GestureBindings; // 手势
// TArray<FInputActionBinding> ActionBindings;   // 动作（项目设置里的 Input Action）

// **PlayerController 只负责管理输入栈（排序）

// PlayerController
// 管理 InputStack
// 按 Priority 排序
// 谁高谁排上面
// InputComponent
// 存绑定（Jump、Move、Fire…）
// 提供 Priority 和 bBlockInput
