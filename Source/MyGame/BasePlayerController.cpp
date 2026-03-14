// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
// BasePlayerController.cpp
#include "BasePlayerController.h"
ABasePlayerController::ABasePlayerController()
{
	// 构造时创建所有 Router（轻量级对象，无性能损耗）

	RoomRouter = nullptr;
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 初始化所有 Router（绑定 PlayerController）
	RoomRouter = NewObject<URoomRouter>(this);
	RoomRouter->Init(this);
}