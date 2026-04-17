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
	AKingHUD(const FObjectInitializer& ObjectInitializer);
	virtual void DrawHUD() override;
	virtual void DrawActorOverlays(FVector Viewpoint, FRotator ViewRotation) override;
public:
	UPROPERTY(EditAnywhere)
	bool bShowActorOverlayInfo=false;

	// 2. 重写回调

	void NotifyHitBoxClick(FName BoxName) override;
	void NotifyHitBoxBeginCursorOver(FName BoxName) override;
	void NotifyHitBoxEndCursorOver(FName BoxName) override;
	void TestHUD();

	// 按钮名字
	const FName Btn_Start = TEXT("Btn_Start");
	const FName Btn_Exit = TEXT("Btn_Exit");

};
