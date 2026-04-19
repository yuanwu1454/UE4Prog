// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GASFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UGASFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// 通用获取当前属性值
	UFUNCTION(BlueprintCallable, Category = "GAS")
	static bool GetAttributeValue(AActor* Actor, const FGameplayAttribute& Attribute, float& OutValue);

	// 专门获取当前Mana
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	static bool GetCurrentMana(AActor* Actor, float& OutMana);

	// 专门获取最大Mana
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	static bool GetMaxMana(AActor* Actor, float& OutMaxMana);

	// 专门获取当前Mana
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	static bool GetCurrentHealth(AActor* Actor, float& OutMana);

	// 专门获取最大Mana
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	static bool GetMaxHealth(AActor* Actor, float& OutMaxMana);

	// GAS 是否初始化
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	static bool IsASCInitialized(AActor* Actor);

	// GAS 是否初始化
	UFUNCTION(BlueprintCallable, Category = "GAS|Attribute")
	static bool IsOwnerBy(AActor* Actor, UAbilitySystemComponent* Component);

	
};
