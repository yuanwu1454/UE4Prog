// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_FireBall.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

UGA_FireBall::UGA_FireBall()
{
	// 技能实例策略：每个Actor一个实例（最常用）
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_FireBall::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 【必须写！第一行就调用】
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// ==============================================
	// 第二步：检查并提交技能（耗蓝、冷却、标签判断）
	// ==============================================
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		// 提交失败，直接结束技能
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// ==============================================
	// 第三步：你的技能逻辑（生成火球）
	// ==============================================
	SpawnFireball();

	// ==============================================
	// 第四步：结束技能（瞬发技能用完就结束）
	// ==============================================
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_FireBall::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_FireBall::SpawnFireball()
{
	if (!FireballClass) return;

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;

	// 获取枪口/技能生成位置
	FVector SpawnLocation = Character->GetMesh()->GetSocketLocation("hand_r");
	FRotator SpawnRot = Character->GetActorRotation();

	// 朝向准星
	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (PC)
	{
		FVector CamLoc;
		FRotator CamRot;
		PC->GetPlayerViewPoint(CamLoc, CamRot);
		SpawnRot = CamRot;
	}

	// 生成火球
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character;
	SpawnParams.Instigator = Character;

	AActor* Fireball = GetWorld()->SpawnActor<AActor>(
		FireballClass,
		SpawnLocation,
		SpawnRot,
		SpawnParams
	);

	// if (Fireball)
	// {
	// 	// --------------------------
	// 	// 关键：火球命中 绑定到 GA 回调
	// 	// --------------------------
	// 	Fireball->OnProjectileHit.BindUObject(this, &UGA_Fireball::OnFireballHit);
	// }
}
void UGA_FireBall::OnFireballHit(AActor* TargetActor)
{
	if (!TargetActor || !DamageEffect) return;

	// 获取双方 ASC
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = TargetActor->FindComponentByClass<UAbilitySystemComponent>();

	if (!SourceASC || !TargetASC) return;

	// 构造伤害 GE
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffect,
		1,  // 技能等级
		SourceASC->MakeEffectContext()
	);

	// 施加伤害
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}