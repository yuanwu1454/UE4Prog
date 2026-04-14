// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDBase.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AHUDBase : public AHUD
{
	GENERATED_BODY()
};


// ShowDebug Reset 重置
// ShowDebug Camera 实时查看相机
// ShowDebug None 关闭

// ShowHUD 切换hud显示

// ShowDebug None 开启debug模式
// 然后 ShowDebugForReticleTargetToggle AHUDOverlayActor 可以绘制指定actor类的边框