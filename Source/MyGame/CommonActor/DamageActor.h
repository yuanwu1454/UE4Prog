// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBase.h"
#include "DamageActor.generated.h"

UCLASS()
class MYGAME_API ADamageActor : public AActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADamageActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
