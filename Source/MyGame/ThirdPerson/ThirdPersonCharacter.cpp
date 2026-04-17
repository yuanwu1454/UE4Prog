// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AS/HeroAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GA/GameplayAbilityBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


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
		
		// UHeroAttributeSet* AttributeSet = NewObject<UHeroAttributeSet>(AbilitySystem);
		// AbilitySystem->AddAttributeSetSubobject(AttributeSet);
		
		GiveDefaultAbilities();

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

	// 1. 构造事件数据
	FGameplayEventData EventData;
	EventData.Instigator = this;          // 发起者
	EventData.Target = this;              // 目标（自己放技能就填自己）
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Ability.Cast.Fireball"));

	// 2. 发送事件给 ASC
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}
