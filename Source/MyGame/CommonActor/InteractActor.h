// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBase.h"
#include "InteractActor.generated.h"

UCLASS()
class MYGAME_API AInteractActor : public AActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractActor();

	// ====================== 组件声明 ======================
	// 1. 根组件：空的场景组件（用来挂载其他组件）
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USceneComponent* RootScene;
	
	// 根组件：静态网格体
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* StaticMesh;

	// 相机组件（附加在StaticMesh上）
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* CameraComponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	virtual void NotifyActorOnReleased(FKey ButtonReleased = EKeys::LeftMouseButton) override;
	virtual void NotifyActorEndCursorOver() override;
	virtual void NotifyActorBeginCursorOver() override;
	virtual void Reset() override;
	virtual void BecomeViewTarget( class APlayerController* PC ) override;
	virtual void EndViewTarget( class APlayerController* PC ) override;

	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
};
