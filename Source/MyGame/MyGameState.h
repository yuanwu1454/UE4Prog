// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Gameplay/MyGameStateBase.h"
#include "MyGameState.generated.h"

/**
* 基础类只保留通用能力：AGameStateBase 是引擎底层的基类，
* 只实现所有游戏都可能用到的通用功能（如时间同步、PlayerState 管理、游戏阶段标记），
* 不会包含具体游戏（如 FPS、MOBA）的业务逻辑；
逻辑下沉到子类 / GameMode：复杂的游戏逻辑要么放在 GameStateBase 的子类（如你自己定义的 AGameState），
要么放在 GameModeBase（服务器独占逻辑），GameState 只负责同步结果；
核心价值是「同步」而非「执行」：GameState 是全复制的（Replicated），
而 GameMode 只存在于服务器。GameState 做的「转发」本质是把服务器 GameMode 的关键状态同步给所有客户端，
这是它最核心的价值，而非「功能少」。
 */
// GameStateBase	服务器 + 所有客户端
UCLASS()
class MYGAME_API AMyGameState : public AMyGameStateBase
{

	GENERATED_BODY()
};
