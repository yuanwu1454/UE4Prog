// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyViewController.h"
#include "UObject/Object.h"
#include "UICommon.h"
#include "MyViewControllerPanel.h"
#include "MyViewControllerPage.generated.h"

UCLASS()
class UMyViewControllerPage : public UMyViewController
{
	GENERATED_BODY()

public:
	void Init(UUserWidget* InRootView);
	void Show();
	void Hide();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUILayer Layer = EUILayer::Normal;

};

// 1. TWeakObjectPtr 支持前置声明
// UE 的弱指针天生支持前置声明，只要你不加 UPROPERTY，就不需要看到类的完整定义。
// 2. UPROPERTY 是万恶之源
// UPROPERTY 会让 UE 反射系统强制解析完整类定义
// 你用了前置声明 class UUIPageBase;，编译器看不到继承关系
// 于是报：无法转换为 UObject*
// 3. TWeakObjectPtr 本身就是 GC 安全的
// 不需要、也不应该用 UPROPERTY 修饰！