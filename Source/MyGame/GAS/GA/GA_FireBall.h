// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "GA_FireBall.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UGA_FireBall : public UGameplayAbilityBase
{
	GENERATED_BODY()
public:
	UGA_FireBall();
	// 技能激活
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 技能结束
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fireball")
	TSubclassOf<AActor> FireballClass; // 火球蓝图类

protected:

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Fireball")
    float FireballSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fireball")
	TSubclassOf<UGameplayEffect> DamageEffect;  // 伤害GE
    // 技能执行
    void SpawnFireball();
	void OnFireballHit(AActor* TargetActor);  // 火球命中回调（在这里打伤害
};
