// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroAttributeSet.h"

#include "GameplayEffectExtension.h"

UHeroAttributeSet::UHeroAttributeSet()
{
	// 初始默认值（可在蓝图/GE里覆盖）
	Health = 100.0f;
	MaxHealth = 100.0f;
	Mana = 50.0f;
	MaxMana = 50.0f;
}

void UHeroAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 调整当前血量时，强制 clamped 在 0 ~ MaxHealth
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	// 调整当前蓝量时，强制 clamped 在 0 ~ MaxMana
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
}

void UHeroAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// 双重保险：GE执行后，再次确保血量不超范围
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	}
}
