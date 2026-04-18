// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBase.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

// 1. 声明代理：写在这里，类外面！
DECLARE_DELEGATE_OneParam(FOnProjectileHit, AActor*);


UCLASS()
class MYGAME_API AProjectileActor : public AActorBase
{
	GENERATED_BODY()
public:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	

	// Sets default values for this actor's properties
	AProjectileActor();

	FOnProjectileHit OnProjectileHit;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	class UStaticMeshComponent* GetStaticMeshComp() ;

	// 随机改变子弹颜色（公用接口）
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void RandomChangeProjectileColor();
};
