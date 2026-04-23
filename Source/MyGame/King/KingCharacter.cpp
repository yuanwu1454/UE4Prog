// Fill out your copyright notice in the Description page of Project Settings.


#include "KingCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "BaseActorComponent.h"
#include "GASInstanceSubsystem.h"
#include "AS/HeroAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GA/GameplayAbilityBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "King/KingAbilitySystemComponent.h"
#include "MyLua/LuaGlobal.h"


// Sets default values
// 无参构造函数自动调用父类
// 有参构造函数需要主动调用
// AChild::AChild(int a) 
// 	: AParent(a)  // 必须手动写
// {}
AKingCharacter::AKingCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	// 1. 创建 CameraBoom（SpringArmComponent）
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent); // 绑定到胶囊体根组件
	// 2. 创建 FollowCamera（UCameraComponent）
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 绑定到 SpringArm 末端

	// 创建ASC
	KingAbilitySystemComponent = CreateDefaultSubobject<UKingAbilitySystemComponent>(TEXT("KingAbilitySystemComponent"));
	// 网络复制（多人必开）
	KingAbilitySystemComponent->SetIsReplicated(true);

	// 用CreateDefaultSubobject创建 AttributeSet 子对象，ASC 会自动识别并注册（无需手动 Add）
	HeroAttributeSet = CreateDefaultSubobject<UHeroAttributeSet>(TEXT("HeroAttributeSet"));
}

void AKingCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);

		// 2. 【关键：绑定属性变化监听】
		ASC->GetGameplayAttributeValueChangeDelegate(
			UHeroAttributeSet::GetHealthAttribute()) // 要监听的属性
			.AddUObject(this, &AKingCharacter::OnHealthChanged);



		ASC->GetGameplayAttributeValueChangeDelegate(
			UHeroAttributeSet::GetManaAttribute()) // 要监听的属性
			.AddUObject(this, &AKingCharacter::OnManaChanged);
		
		
		Handle_CDStart = ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &AKingCharacter::OnCooldownStarted);

		// ==============================================
		// 2. 冷却结束（GE移除）
		// ==============================================
		Handle_CDEnd = ASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &AKingCharacter::OnCooldownEnded);
		
		
		GiveDefaultAbilities();

		LuaGlobal::CallVoidLua("NDCall", "InitASCNotify", ASC);
	}
	
}

void AKingCharacter::UnPossessed()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		// 解绑属性
		ASC->GetGameplayAttributeValueChangeDelegate(UHeroAttributeSet::GetHealthAttribute()).RemoveAll(this);
		ASC->GetGameplayAttributeValueChangeDelegate(UHeroAttributeSet::GetManaAttribute()).RemoveAll(this);

		// 解绑冷却委托
		if (Handle_CDStart.IsValid())
		{
			ASC->OnActiveGameplayEffectAddedDelegateToSelf.Remove(Handle_CDStart);
			Handle_CDStart.Reset();
		}
		if (Handle_CDEnd.IsValid())
		{
			ASC->OnAnyGameplayEffectRemovedDelegate().Remove(Handle_CDEnd);
			Handle_CDEnd.Reset();
		}

		// 3. 服务器专属：停止所有技能、清空授予技能
		if (ASC->IsOwnerActorAuthoritative())
		{
			ASC->CancelAllAbilities();    // 终止正在释放的技能
			ASC->ClearAllAbilities();     // 清空Give的所有技能
		}

		// 4. 【关键】UE4官方清空AbilityActorInfo 接口
		ASC->ClearActorInfo();
	}
	Super::UnPossessed();
}

UAbilitySystemComponent* AKingCharacter::GetAbilitySystemComponent() const
{
	return GetKingAbilitySystemComponent();
}

// Called when the game starts or when spawned
void AKingCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called to bind functionality to input
void AKingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	// 按下 Left Shift → 奔跑
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AKingCharacter::StartSprint);
	// 松开 Left Shift → 走路
	InputComponent->BindAction("Sprint", IE_Released, this, &AKingCharacter::StopSprint);

	// 跳跃（空格）
	InputComponent->BindAction("Jump", IE_Pressed, this, &AKingCharacter::JumpPressed);
	InputComponent->BindAction("Jump", IE_Released, this, &AKingCharacter::JumpReleased);
	
	InputComponent->BindKey(FKey("Q"), IE_Released, this, &AKingCharacter::OnFireballInputPressed);

	InputComponent->BindKey(FKey("N"), IE_Released, this, &AKingCharacter::TestAddFunc);
}

// ==============================================
// 开始奔跑
// ==============================================
void AKingCharacter::StartSprint()
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
void AKingCharacter::StopSprint()
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
void AKingCharacter::JumpPressed()
{
	Jump(); // 父类 ACharacter 自带跳跃！直接调用就行
}

void AKingCharacter::JumpReleased()
{
	StopJumping(); // 父类自带，松开停止跳跃（控制跳跃高度）
}

void AKingCharacter::TestAddFunc()
{
	// 先找身上有没有
	UBaseActorComponent* ExistComp = FindComponentByClass<UBaseActorComponent>();

	if (ExistComp)
	{
		ExistComp->DestroyComponent();
		UE_LOG(LogTemp, Log, TEXT("删除已有组件"));
	}
	else
	{
		UBaseActorComponent* NewComp = NewObject<UBaseActorComponent>(this);
		NewComp->RegisterComponent();
		NewComp->Activate();
		UE_LOG(LogTemp, Log, TEXT("创建新组件"));
	}
}

void AKingCharacter::GiveDefaultAbilities()
{
	if (!HasAuthority() || !KingAbilitySystemComponent) return;

	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultAbilities)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec Spec(AbilityClass, 1);
			KingAbilitySystemComponent->GiveAbility(Spec);
		}
	}
}

void AKingCharacter::OnFireballInputPressed()
{
	
	if (!KingAbilitySystemComponent) return;

	if(auto Inst = UGASInstanceSubsystem::Get(this))
	{
		Inst->ShowASCTag(KingAbilitySystemComponent);
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
void AKingCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("血量：%f → %f"), Data.OldValue, Data.NewValue);
	LuaGlobal::CallVoidLua("NDCall", "HealthNotify", KingAbilitySystemComponent, Data.OldValue, Data.NewValue);
}

// 蓝量变化
void AKingCharacter::OnManaChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("蓝量：%f → %f"), Data.OldValue, Data.NewValue);
	LuaGlobal::CallVoidLua("NDCall", "ManaNotify", KingAbilitySystemComponent, Data.OldValue, Data.NewValue);
}

void AKingCharacter::OnCooldownStarted(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec,
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

void AKingCharacter::OnCooldownEnded(const FActiveGameplayEffect& Effect)
{
	if (Effect.Spec.Def->InheritableOwnedTagsContainer.CombinedTags.HasTag(CooldownTag))
	{
		// ---------------- 打印冷却结束 ----------------
		UE_LOG(LogTemp, Log, TEXT("====================================="));
		UE_LOG(LogTemp, Log, TEXT("✅ 冷却结束！技能可用"));
		UE_LOG(LogTemp, Log, TEXT("====================================="));
	}
}
