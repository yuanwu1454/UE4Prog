// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilityBase.h"

#include "AbilitySystemComponent.h"
#include "GASInstanceSubsystem.h"

void UGameplayAbilityBase::ShowASCTag(UAbilitySystemComponent* ASC)
{
	if(auto Inst = UGASInstanceSubsystem::Get(this))
	{
		Inst->ShowASCTag(ASC);
	}
}

void UGameplayAbilityBase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == EKingAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
		
	}
	else if(AbilityActivationPolicy == EKingAbilityActivationPolicy::OnTriggered)
	{
		
	}
}

void UGameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (AbilityActivationPolicy == EKingAbilityActivationPolicy::OnGiven)
	{
		ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
	}
	else if(AbilityActivationPolicy == EKingAbilityActivationPolicy::OnTriggered)
	{
		
	}

}
