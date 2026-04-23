// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponentBase.h"
#include "Gameplay/BaseCharacter.h"
#include "KingCharacter.generated.h"

class UHeroAttributeSet;
class UKingAbilitySystemComponent;

UCLASS()
class MYGAME_API AKingCharacter : public ABaseCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKingCharacter();


	// 【关键】暴露给蓝图的技能列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS | Abilities")
	TArray<TSubclassOf<class UGameplayAbilityBase>> DefaultAbilities;
	

	FORCEINLINE UKingAbilitySystemComponent* GetKingAbilitySystemComponent() const {return KingAbilitySystemComponent;}
	FORCEINLINE UHeroAttributeSet* GetHeroAttributeSet() const {return HeroAttributeSet;}
protected:

	// ~Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	// ~End APawn Interface

	// ~Begin IAbilitySystemInterface Interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~End IAbilitySystemInterface Interface
	
	// ASC 组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UKingAbilitySystemComponent* KingAbilitySystemComponent;
	// ASC 组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UHeroAttributeSet* HeroAttributeSet;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 组件声明（对应你蓝图里的 CameraBoom 和 FollowCamera）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	// 是否正在奔跑
	bool bIsSprinting = false;
	
	// 奔跑速度（蓝图可改）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 600.0f;

	// 正常走路速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 300.0f;
	
	// 开始奔跑
	void StartSprint();

	// 停止奔跑
	void StopSprint();

	// 按下跳跃
	void JumpPressed();
	// 松开跳跃
	void JumpReleased();

	void TestAddFunc();

	// 自动授予技能（内部调用）
	void GiveDefaultAbilities();

	void OnFireballInputPressed();

	// 委托句柄
	FDelegateHandle Handle_CDStart;
	FDelegateHandle Handle_CDEnd;
	
	// 用来保存委托句柄（必须存，不然会失效）
	FOnAttributeChangeData OnAttributeChangedHandle;
	// 你的技能冷却Tag（自己改）
	UPROPERTY(EditAnywhere)
	FGameplayTag CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Fireball"));
		
    // 属性变化回调
    void OnHealthChanged(const FOnAttributeChangeData& Data);
    void OnManaChanged(const FOnAttributeChangeData& Data);
	void OnCooldownStarted(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	void OnCooldownEnded(const FActiveGameplayEffect& Effect);
public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
