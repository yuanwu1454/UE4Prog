// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/PlayerControllerBase.h"
#include "KingPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AKingPlayerController : public APlayerControllerBase
{
	GENERATED_BODY()
	AKingPlayerController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	// 绑定 C 键触发射线检测
	void Press_C_Button();

	// 射线检测 屏幕中心
	bool LineTraceFromScreenCenter(FHitResult& HitResult);

	// 保存原来的视角，方便切回去
	UPROPERTY(Transient)
	AActor* OriginalViewTarget =nullptr;
	UPROPERTY(Transient)
	AActor* CurrentFocusedActor = nullptr;
};
