// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GASInstanceSubsystem.h"
#include "AS/HeroAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GA/GameplayAbilityBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "MyLua/LuaGlobal.h"


// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	// 1. 创建 CameraBoom（SpringArmComponent）
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent); // 绑定到胶囊体根组件
	// 2. 创建 FollowCamera（UCameraComponent）
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 绑定到 SpringArm 末端

	// 创建ASC
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>(TEXT("AbilitySystem"));
	// 网络复制（多人必开）
	AbilitySystemComponent->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	// 初始化ASC（Owner和Avatar都是自己）
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		
		AbilitySystemComponent->AddSet<UHeroAttributeSet>();

		// 2. 【关键：绑定属性变化监听】
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UHeroAttributeSet::GetHealthAttribute()) // 要监听的属性
			.AddUObject(this, &AThirdPersonCharacter::OnHealthChanged);



		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UHeroAttributeSet::GetManaAttribute()) // 要监听的属性
			.AddUObject(this, &AThirdPersonCharacter::OnManaChanged);
		
		
			Handle_CDStart = AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &AThirdPersonCharacter::OnCooldownStarted);

		// ==============================================
		// 2. 冷却结束（GE移除）
		// ==============================================
		Handle_CDEnd = AbilitySystemComponent->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &AThirdPersonCharacter::OnCooldownEnded);
		
		
		GiveDefaultAbilities();

		LuaGlobal::CallVoidLua("NDCall", "InitASCNotify", AbilitySystemComponent);

	}
}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	// 按下 Left Shift → 奔跑
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AThirdPersonCharacter::StartSprint);
	// 松开 Left Shift → 走路
	InputComponent->BindAction("Sprint", IE_Released, this, &AThirdPersonCharacter::StopSprint);

	// 跳跃（空格）
	InputComponent->BindAction("Jump", IE_Pressed, this, &AThirdPersonCharacter::JumpPressed);
	InputComponent->BindAction("Jump", IE_Released, this, &AThirdPersonCharacter::JumpReleased);
	
	InputComponent->BindKey(FKey("Q"), IE_Released, this, &AThirdPersonCharacter::OnFireballInputPressed);
}

// ==============================================
// 开始奔跑
// ==============================================
void AThirdPersonCharacter::StartSprint()
{
	bIsSprinting = true;
	// 直接用官方自带的 GetCharacterMovement() ！！！
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement)
	{
		Movement->MaxWalkSpeed = SprintSpeed;
	}
	
}

// ==============================================
// 停止奔跑
// ==============================================
void AThirdPersonCharacter::StopSprint()
{
	bIsSprinting = false;
	// 直接用官方自带的 GetCharacterMovement() ！！！
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement)
	{
		Movement->MaxWalkSpeed = WalkSpeed;
	}
}


// ==================== 跳跃 ====================
void AThirdPersonCharacter::JumpPressed()
{
	Jump(); // 父类 ACharacter 自带跳跃！直接调用就行
}

void AThirdPersonCharacter::JumpReleased()
{
	StopJumping(); // 父类自带，松开停止跳跃（控制跳跃高度）
}

void AThirdPersonCharacter::GiveDefaultAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultAbilities)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec Spec(AbilityClass, 1);
			AbilitySystemComponent->GiveAbility(Spec);
		}
	}
}

void AThirdPersonCharacter::OnFireballInputPressed()
{
	
	if (!AbilitySystemComponent) return;

	if(auto Inst = UGASInstanceSubsystem::Get(this))
	{
		Inst->ShowASCTag(AbilitySystemComponent);
	}

	// 1. 构造事件数据
	FGameplayEventData EventData;
	EventData.Instigator = this;          // 发起者
	EventData.Target = this;              // 目标（自己放技能就填自己）
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Ability.Cast.Fireball"));

	// 2. 发送事件给 ASC
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}


// 血量变化
void AThirdPersonCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("血量：%f → %f"), Data.OldValue, Data.NewValue);
	LuaGlobal::CallVoidLua("NDCall", "HealthNotify", AbilitySystemComponent, Data.OldValue, Data.NewValue);
}

// 蓝量变化
void AThirdPersonCharacter::OnManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("蓝量：%f → %f"), Data.OldValue, Data.NewValue);
	LuaGlobal::CallVoidLua("NDCall", "ManaNotify", AbilitySystemComponent, Data.OldValue, Data.NewValue);
}

void AThirdPersonCharacter::OnCooldownStarted(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec,
	FActiveGameplayEffectHandle Handle)
{
	if (Spec.Def->InheritableOwnedTagsContainer.CombinedTags.HasTag(CooldownTag))
	{
		const FActiveGameplayEffect* ActiveGE = ASC->GetActiveGameplayEffect(Handle);
		if (ActiveGE)
		{
			float CurrentTime = ASC->GetWorld()->GetTimeSeconds();
			float TimeRemaining = ActiveGE->GetTimeRemaining(CurrentTime);
			float TotalDuration = ActiveGE->GetDuration();

			UE_LOG(LogTemp, Log, TEXT("====================================="));
			UE_LOG(LogTemp, Log, TEXT("🔥 冷却开始"));
			UE_LOG(LogTemp, Log, TEXT("✅ 剩余时间: %.2f 秒"), TimeRemaining);
			UE_LOG(LogTemp, Log, TEXT("✅ 总冷却时间: %.2f 秒"), TotalDuration);
			UE_LOG(LogTemp, Log, TEXT("====================================="));
		}		
	}
}

void AThirdPersonCharacter::OnCooldownEnded(const FActiveGameplayEffect& Effect)
{
	if (Effect.Spec.Def->InheritableOwnedTagsContainer.CombinedTags.HasTag(CooldownTag))
	{
		// ---------------- 打印冷却结束 ----------------
		UE_LOG(LogTemp, Log, TEXT("====================================="));
		UE_LOG(LogTemp, Log, TEXT("✅ 冷却结束！技能可用"));
		UE_LOG(LogTemp, Log, TEXT("====================================="));
	}
}
