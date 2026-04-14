// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBase.h"
#include "GameFramework/Actor.h"
#include "HUDOverlayActor.generated.h"

UCLASS()
class MYGAME_API AHUDOverlayActor : public AActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHUDOverlayActor();

	UPROPERTY(EditAnywhere)
	bool bShowInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// 引擎官方就是用这个函数画Overlay！
	virtual void PostRenderFor(APlayerController* PC, UCanvas* Canvas, FVector CameraPosition, FVector CameraDir) override;

};
