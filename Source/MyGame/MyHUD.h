// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/HUDBase.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AMyHUD : public AHUDBase
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
    // 软引用UI类（蓝图可编辑，路径可配置）
public:

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
};
