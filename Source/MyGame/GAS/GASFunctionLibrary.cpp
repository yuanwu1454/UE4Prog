// Fill out your copyright notice in the Description page of Project Settings.


#include "GASFunctionLibrary.h"

#include "AbilitySystemComponent.h"
#include "AS/HeroAttributeSet.h"

bool UGASFunctionLibrary::GetAttributeValue(AActor* Actor, const FGameplayAttribute& Attribute, float& OutValue)
{
	OutValue = 0.f;

	if (!Actor) return false;
	
	UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(Actor->GetComponentByClass(UAbilitySystemComponent::StaticClass()));
	if (!ASC) return false;
	
	if (Attribute.IsValid())
	{
		OutValue = ASC->GetNumericAttribute(Attribute);
		return true;
	}
	return false;
}

bool UGASFunctionLibrary::GetCurrentMana(AActor* Actor, float& OutMana)
{
	return GetAttributeValue(Actor, UHeroAttributeSet::GetManaAttribute(), OutMana);
}

bool UGASFunctionLibrary::GetMaxMana(AActor* Actor, float& OutMaxMana)
{
	return GetAttributeValue(Actor, UHeroAttributeSet::GetMaxManaAttribute(), OutMaxMana);
}

bool UGASFunctionLibrary::GetCurrentHealth(AActor* Actor, float& OutMana)
{
	return GetAttributeValue(Actor, UHeroAttributeSet::GetHealthAttribute(), OutMana);
}

bool UGASFunctionLibrary::GetMaxHealth(AActor* Actor, float& OutMaxMana)
{
	return GetAttributeValue(Actor, UHeroAttributeSet::GetMaxHealthAttribute(), OutMaxMana);
}

bool UGASFunctionLibrary::IsASCInitialized(AActor* Actor)
{
	if (!Actor) return false;
	
	UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(Actor->GetComponentByClass(UAbilitySystemComponent::StaticClass()));
	if (!ASC) return false;
	
	return ASC->AbilityActorInfo.IsValid();
}

bool UGASFunctionLibrary::IsOwnerBy(AActor* Actor, UAbilitySystemComponent* Component)
{
	if (Actor)
	{
		UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(Actor->GetComponentByClass(UAbilitySystemComponent::StaticClass()));
		if(ASC)
		{
			return ASC == Component;			
		}
	}
	return false;

}
