// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/HUDBase.h"
#include "KingHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AKingHUD : public AHUDBase
{
	GENERATED_BODY()
	// 重写 DrawHUD —— 所有 2D 绘制都在这里
	virtual void DrawHUD() override;
};
