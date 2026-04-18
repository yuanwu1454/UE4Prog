// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "GameFramework/HUD.h"
#include "HUDBase.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AHUDBase : public AHUD, public IUnLuaInterface
{
	GENERATED_BODY()
public:
	virtual FString GetModuleName_Implementation() const override;

	// Lua文件路径，不需要扩展名
	UPROPERTY(EditDefaultsOnly, AssetRegistrySearchable, Category = "Config | Lua")
	FString LuaModuleName;

};


// ShowDebug Reset 重置
// ShowDebug Camera 实时查看相机
// ShowDebug None 关闭

// ShowHUD 切换hud显示

// ShowDebug None 开启debug模式
// 然后 ShowDebugForReticleTargetToggle AHUDOverlayActor 可以绘制指定actor类的边框