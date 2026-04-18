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
