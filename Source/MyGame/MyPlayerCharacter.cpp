// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerCharacter.h"

#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Log/MultiplayerLogHelper.h"
#include "Net/UnrealNetwork.h"
#include "MyProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

AMyPlayerCharacter::AMyPlayerCharacter()
{
	// 启用网络复制
	bReplicates = true;
	bAlwaysRelevant = true;

		// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	// R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	// R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	// R_MotionController->SetupAttachment(RootComponent);
	// L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	// L_MotionController->SetupAttachment(RootComponent);
	//
	// // Create a gun and attach it to the right-hand VR controller.
	// // Create a gun mesh component
	// VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	// VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	// VR_Gun->bCastDynamicShadow = false;
	// VR_Gun->CastShadow = false;
	// VR_Gun->SetupAttachment(R_MotionController);
	// VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	//
	// VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	// VR_MuzzleLocation->SetupAttachment(VR_Gun);
	// VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	// VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

// 第一步：声明要同步的属性（仅MoveSpeed）
void AMyPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 仅注册MoveSpeed为同步属性，TempFPS不注册
	DOREPLIFETIME(AMyPlayerCharacter, MoveSpeed);
}

// 第二步：重写Serialize，仅序列化需要同步的属性
void AMyPlayerCharacter::Serialize(FArchive& Ar)
{
	// 判断：仅在「网络同步归档」时优化，其他场景（如保存）仍调用父类
	if (Ar.IsNetArchive()) // 关键：只优化网络同步的序列化
		{
		// 仅序列化需要同步的核心属性（MoveSpeed）
		// 通过相关状态 来区分指定情况下 是否同步指定属性，这样可以减轻数据带宽
		Ar << MoveSpeed;
		UE_LOG(LogTemp, Log, TEXT("网络同步：仅序列化MoveSpeed=%.1f"), MoveSpeed);
		}
	else
	{
		// 非网络场景（如保存UAsset/本地序列化），正常调用父类
		Super::Serialize(Ar);
	}
}

// 测试方法：修改属性（模拟游戏中实时变化）
void AMyPlayerCharacter::UpdateAttributes(float NewSpeed, int32 NewFPS)
{
	if (HasAuthority()) // 单机/服务器才允许修改
	{
		MoveSpeed = NewSpeed;
		TempFPS = NewFPS;
	}
	else
	{
		// 客户端请求服务器修改（通过RPC）
		Server_UpdateAttributes(NewSpeed, NewFPS);
	}
}

void AMyPlayerCharacter::UpdateMoveSpeed(float NewSpeed)
{
	if (HasAuthority()) // 单机/服务器才允许修改
		{
		MoveSpeed = NewSpeed;
		}
	else
	{
		// 客户端请求服务器修改（通过RPC）
		Server_UpdateMoveSpeed(NewSpeed);
	}
}

void AMyPlayerCharacter::Server_UpdateMoveSpeed_Implementation(float NewSpeed)
{
	MoveSpeed = NewSpeed;
}


// ------------ 规则2：Server RPC 的实现函数（必写，后缀 _Implementation） ------------
// 作用：服务器端实际执行的逻辑（修改属性、同步数据）
void AMyPlayerCharacter::Server_UpdateAttributes_Implementation(float NewSpeed, int32 NewFPS)
{
	// 服务器端修改属性（会自动同步到客户端，因为 MoveSpeed 标记了 Replicated）
	MoveSpeed = NewSpeed;
	TempFPS = NewFPS;

	UE_LOG(LogTemp, Log, TEXT("服务器执行 RPC：修改 MoveSpeed=%.1f, TempFPS=%d"), MoveSpeed, TempFPS);
}

// ------------ 属性同步回调 ------------
void AMyPlayerCharacter::OnRep_MoveSpeed()
{
	UE_LOG(LogTemp, Log, TEXT("客户端同步 MoveSpeed：%.1f"), MoveSpeed);
}

void AMyPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 记录新客户端登录，传入PlayerController作为PlayerContext
	MULTI_LOG(FString::Printf(TEXT("PossessedBy：%s"), *this->GetName()), this, this);
}


// 打开技能面板
void AMyPlayerCharacter::OpenSkillPanel()
{
	if (!bIsSkillPanelOpen)
	{
		bIsSkillPanelOpen = true;

		// 2. 创建UI专用的InputComponent
		SkillPanelInputComp = NewObject<UInputComponent>(this);
		SkillPanelInputComp->RegisterComponent(); // 必须注册才能生效

		// 3. 绑定UI的输入响应
		// 关闭面板（ESC）
		SkillPanelInputComp->BindAction("CloseSkillPanel", IE_Pressed, this, &AMyPlayerCharacter::CloseSkillPanel);

		// 4. 将UI的InputComponent推到输入栈顶（优先级最高）
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->PushInputComponent(SkillPanelInputComp);
			UE_LOG(LogTemp, Warning, TEXT("技能面板打开，UI输入组件已置顶"));
		}
	}
}

// 关闭技能面板
void AMyPlayerCharacter::CloseSkillPanel()
{
	if (bIsSkillPanelOpen)
	{
		bIsSkillPanelOpen = false;

		// 5. 从输入栈中移除UI的InputComponent
		if (APlayerController* PC = Cast<APlayerController>(this))
		{
			bool bRemoved = PC->PopInputComponent(SkillPanelInputComp);
			if (bRemoved)
			{
				UE_LOG(LogTemp, Warning, TEXT("技能面板关闭，UI输入组件已移除"));
			}
		}

		// 销毁UI的InputComponent（可选，也可复用）
		if (SkillPanelInputComp)
		{
			SkillPanelInputComp->UnregisterComponent();
			SkillPanelInputComp = nullptr;
		}
	}
}

bool AMyPlayerCharacter::IsSkillKeyPressed(FName KeyName)
{
	// 1. 从PlayerController获取PlayerInput
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->PlayerInput)
	{
		// 2. 查询指定按键的按下状态（支持动作名/轴名/原始按键名）
		return PC->PlayerInput->IsPressed(KeyName);
	}
	return false;
}

void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(false, true);
}

void AMyPlayerCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<AMyProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}
}
	
void AMyPlayerCharacter::MoveForward(float Value)
{
		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorForwardVector(), Value);
		}
}

void AMyPlayerCharacter::MoveRight(float Value)
{	if (Value != 0.0f)
{
	// add movement in that direction
	AddMovementInput(GetActorRightVector(), Value);
}
}

void AMyPlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyPlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
	
void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyPlayerCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyPlayerCharacter::LookUpAtRate);
}

