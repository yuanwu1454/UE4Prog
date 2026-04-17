// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
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

	
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	// 按下 Left Shift → 奔跑
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AThirdPersonCharacter::StartSprint);
	// 松开 Left Shift → 走路
	InputComponent->BindAction("Sprint", IE_Released, this, &AThirdPersonCharacter::StopSprint);
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
