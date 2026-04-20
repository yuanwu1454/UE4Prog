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


// 维度	Game 模式（游戏输入）	UI 模式（UMG/Slate）
// 核心系统	PlayerInput + InputComponent	Slate + UMG Widget
// 事件来源	引擎输入驱动（每帧 Tick）	操作系统消息
// 优先级	默认低于 UI	默认高于游戏
// 响应机制	输入栈（从上到下）	焦点 + 层级 + 冒泡
// 数据类型	动作（Action）+ 轴值（Axis）	点击、输入、焦点、滑动
// 绑定方式	C++/ 蓝图绑定按键	控件事件（OnClicked）
// 暂停行为	可被游戏暂停冻结	通常不受暂停影响
// 消费行为	ConsumeKey / BlockInput	SetHandleAndConsume
// 控制目标	Pawn、角色、相机	按钮、输入框、菜单

// game模式优先级
// 自己插入的inputcomponent， playercontroller, scirptActor, pawn's inputcomponent