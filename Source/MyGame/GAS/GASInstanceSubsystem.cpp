// Fill out your copyright notice in the Description page of Project Settings.


#include "GASInstanceSubsystem.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"

UGASInstanceSubsystem* UGASInstanceSubsystem::Get(UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GI ? GI->GetSubsystem<UGASInstanceSubsystem>() : nullptr;
}

void UGASInstanceSubsystem::ShowASCTag(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;
	UE_LOG(LogTemp, Warning, TEXT("=== 当前角色身上的所有标签 ==="));
	FGameplayTagContainer CurrentTags;
	ASC->GetOwnedGameplayTags(CurrentTags);
	for (const FGameplayTag& Tag : CurrentTags)
	{
		UE_LOG(LogTemp, Warning, TEXT("标签: %s"), *Tag.ToString());
	}
}
