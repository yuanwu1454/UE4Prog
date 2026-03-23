// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	// 初始化唯一标识（服务器执行）
	virtual void InitializePlayerState(int32 InPlayerId, const FString& InPlayerName = TEXT(""));

	// 获取带ID的唯一名称（如 "Player_01[ID:1]"）
	UFUNCTION(BlueprintCallable)
	FString GetUniquePlayerName() const;

	// 引擎自增唯一ID（服务器分配，同步到客户端）
	UPROPERTY(Replicated)
	int32 UniquePlayerId;

	// 自定义玩家昵称（可选，同步到客户端）
	UPROPERTY(Replicated)
	FString CustomPlayerName;

protected:
	// 声明需要复制的属性（必须！否则客户端拿不到）
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

// 服务器
// 拥有所有 PlayerState 的权威数据
// 只有服务器可以修改 PlayerState
// 客户端
// 拥有所有 PlayerState 的副本
// 只能读取，不能直接修改
// 数据由服务器自动同步过来

// GameState->PlayerArray[0];

// 1. PlayerId (int32)
// 你现在看到的这个
// 一局游戏内唯一
// 重新开局会变
// 2. Platform UniqueNetId (FUniqueNetId)
// 平台唯一 ID（Steam、Epic、QQ 游戏大厅等）

// bShouldUpdateReplicatedPing
// = 是否自动同步玩家的 Ping 值给客户端

// bIsSpectator	当前是不是在观战	临时状态（死了观战，复活变回玩家）
// bOnlySpectator	永久只能当观察者	固定身份（永远不能玩，只能看）
//
// bIsABot = 标记这个玩家是不是 AI / 机器人
// 由服务器自动赋值
// 网络同步给所有客户端
// 用来区分真人玩家和AI 机器人

// bHasBeenWelcomed 基本不用
// IsInactive() = 玩家是否已退出 / 离线

// bFromPreviousLevel
// 判断是否是上一张地图遗留的临时PlayerState
//
// StartTime = 这个玩家的 PlayerState 在服务器上被创建时的服务器时间戳 **（秒）**
// 记录玩家何时加入本局游戏
// 服务器时间，不受客户端时间影响
// 网络同步给所有客户端

// 1. 翻译 + 作用
// EngineMessageClass
// = 用来发送通用、可本地化、跨游戏的客户端提示消息
// 作用：显示系统提示（玩家加入、退出、欢迎、游戏提示）
// 类型：ULocalMessage 类的子类
// 特点：引擎内置，自动本地化（多语言）
// 开发基本不用

// ExactPing / ExactPingV2 = 引擎内部算 Ping 的原始数据
// 不同步、不对外、业务逻辑永不用
// 游戏里获取延迟，只认 GetPing ()

// 必须用 UniqueId 的场景
// 玩家重连识别身份
// 账号数据保存（存档、战绩、皮肤）
// 封禁玩家、黑名单
// 跨局、跨房间识别玩家
// 在线子系统（OSS）所有接口都要它

// SessionName
// 3. 关键特性（必须知道）
// 类型：FName
// 快速索引的字符串，UE 标准名称类型
// 没有 Replicated
// ❌ 不网络同步
// 只在服务器有效
// 在线子系统 OSS 专用
// 给引擎底层房间管理用
// 游戏业务逻辑几乎不用
// 你正常开发根本碰不到它

// DispatchOverrideWith
// DispatchCopyProperties
// 1. DispatchOverrideWith
// 注释直译：调用 OverrideWith 并触发蓝图扩展的 OnOverrideWith
// 功能：
// 执行 C++ 层的 OverrideWith 逻辑（覆盖 / 替换 PlayerState 数据）
// 触发蓝图可绑定的事件 OnOverrideWith，让蓝图层能扩展逻辑
// 典型场景：玩家重新连接、切换角色、状态重置时覆盖旧的 PlayerState 数据
// 本质：C++ 调用 + 蓝图事件触发的分发器
// 2. DispatchCopyProperties
// 功能：
// 执行 PlayerState 之间的属性复制（血量、分数、道具等）
// 用于状态同步、玩家重生、多端数据一致性
// 典型场景：
// 玩家死亡重生 → 复制旧状态到新 PlayerState
// 多玩家联网 → 服务器同步数据到客户端
// 角色切换 → 继承状态属性
// 两个都是封装分发函数，用于 C++ 逻辑 + 蓝图扩展，是 UE 联网游戏中 PlayerState 管理的标准写法。