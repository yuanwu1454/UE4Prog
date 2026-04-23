// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityBase.generated.h"

UENUM(BlueprintType)
enum class EKingAbilityActivationPolicy:uint8
{
	OnTriggered,
	OnGiven
};

/**
 * 
 */
UCLASS()
class MYGAME_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
public:
	void ShowASCTag(UAbilitySystemComponent* ASC);

protected:
	//~ Begin UGameplayAbility Interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End UGameplayAbility Interface
	UPROPERTY(EditDefaultsOnly, Category="KingAbiliity")
	EKingAbilityActivationPolicy AbilityActivationPolicy = EKingAbilityActivationPolicy::OnTriggered;
};
