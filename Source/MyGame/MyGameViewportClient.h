// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "MyGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
public:
	UMyGameViewportClient();
	UMyGameViewportClient(const FObjectInitializer& ObjectInitializer);
	virtual void ReceivedFocus(FViewport* InViewport) override; // 获得焦点
	virtual void LostFocus(FViewport* InViewport) override;     // 失去焦点
};

// 你说得完全对！在编辑器（PIE）内运行时，ReceivedFocus / LostFocus 经常不触发，
// 甚至你的 UMyGameViewportClient 根本没被创建 / 使用。
// 这是 UE4.27 最经典的坑：编辑器内 PIE 与独立游戏，用的不是同一套 Viewport 体系。