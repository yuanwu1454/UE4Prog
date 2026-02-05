// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// 自定义游戏模块类：继承FGameModuleBase，重写启动/关闭函数
class FMyGameModule : public IModuleInterface
{
public:
	// 重写：游戏启动时执行（模块加载）【核心】
	virtual void StartupModule() override;

	// 重写：游戏关闭时执行（模块卸载）【核心】
	virtual void ShutdownModule() override;
};
