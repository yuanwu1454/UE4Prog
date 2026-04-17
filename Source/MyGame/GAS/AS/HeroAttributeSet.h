// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetBase.h"
#include "HeroAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// 生成静态属性获取函数（GAS 内部用）
// 生成获取值 GetXX()
// 生成设置值 SetXX()
// 生成初始化值 InitXX()

// // 获取属性集
// UHeroAttributeSet* Attributes = AbilitySystem->GetSet<UHeroAttributeSet>();
//
// // 用自动生成的函数
// float CurrentHP = Attributes->GetHealth();
// Attributes->SetHealth(CurrentHP - 10.0f);
// Attributes->InitHealth(100.0f);

UCLASS()
class MYGAME_API UHeroAttributeSet : public UAttributeSetBase
{
	GENERATED_BODY()

public:
	UHeroAttributeSet();
	// ====================== 生命值 ======================
	// 当前血量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UHeroAttributeSet, Health)

	// 最大血量（必须有！）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHeroAttributeSet, MaxHealth)

	// ====================== 魔法值 ======================
	// 当前蓝量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UHeroAttributeSet, Mana)

	// 最大蓝量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UHeroAttributeSet, MaxMana)
protected:
	// 属性修改前调用：限制血量/蓝量在 0 ~ 最大值之间
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// 属性修改后调用：同步当前值不超过最大值（更安全）
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
