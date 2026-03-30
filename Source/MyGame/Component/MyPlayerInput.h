// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerInput.h"
#include "MyPlayerInput.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyPlayerInput : public UPlayerInput
{
	GENERATED_BODY()
};

// UPlayerInput
// 真正干活的！
// 遍历栈
// 检查按键
// 触发绑定
// 执行你的函数
// 处理阻塞
