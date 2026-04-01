// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyGameData.h"
#include "MyGlobals.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, config=Engine)
class MYGAME_API UMyGlobals : public UObject
{
	GENERATED_BODY()
public:
	static UMyGlobals& Get();
	static UMyGlobals* SafeGet();
	UMyGameData* GetGameData() const
	{
		return GameData;
	}

protected:
	UPROPERTY()
	UMyGameData* GameData;
};

// 二、它在引擎里的真实位置
// 它属于 UEngine 类！
// cpp
// 运行
// UEngine::GameSingleton
// 也就是说：
// 它是整个引擎级别的单例，不是游戏级别的！
// 比：
// GameInstance
// PlayerController
// GameMode
// World
// 都更顶层、更全局、生命周期更长。
// 四、它能干嘛？（你最关心的用途）
// ✅ 全局唯一实例
// ✅ 贯穿所有地图、所有世界、所有游戏实例
// ✅ 不用依赖 GameInstance
// ✅ 不用依赖 PlayerController
// ✅ 加载地图不销毁
// ✅ 比任何对象都早创建
// ✅ 比任何对象都晚销毁
// 五、最实用用途（你可以直接用）
// 全局配置管理器
// 热更新流程管理器
// 启动流程检查
// 全局日志系统
// 全局 SDK 初始化（微信、登录、支付、引擎插件）
// 跨 World 共享数据（不依赖 GameInstance）
// 底层渲染 / 音频 / 输入全局管理