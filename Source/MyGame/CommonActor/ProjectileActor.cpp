// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileActor.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AProjectileActor::AProjectileActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectileActor::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{

	// GetStaticMeshComp();

	RandomChangeProjectileColor();
	
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}

UStaticMeshComponent* AProjectileActor::GetStaticMeshComp()
{
	// 找到第一个StaticMesh
	UStaticMeshComponent* MyStaticMesh = nullptr;
	// {
	// 	// 获取挂载在 CollisionComp 下的 StaticMesh 组件
	// 	
	// 	// 遍历碰撞体的所有子组件，找到 StaticMesh
	// 	TArray<USceneComponent*> TempChildren;
	// 	CollisionComp->GetChildrenComponents(true, TempChildren);
	//
	// 	for (USceneComponent* Child : TempChildren)
	// 	{
	// 		if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Child))
	// 		{
	// 			MyStaticMesh = MeshComp;
	// 			break; // 找到第一个就停止
	// 		}
	// 	}
	//
	// 	// 找到了！
	// 	if (MyStaticMesh)
	// 	{
	// 		UE_LOG(LogTemp, Log, TEXT("A 成功获取蓝图中的StaticMesh: %s"), *MyStaticMesh->GetName());
	// 		// 你可以在这里操作它：
	// 		// MyStaticMesh->SetVisibility(false);
	// 		// MyStaticMesh->SetStaticMesh(NewMesh);
	// 	}
	// 	
	// }

	// {
	// 	// 如果你的子弹蓝图里 只有一个 StaticMesh，用这个更简单：
	// 	MyStaticMesh = FindComponentByClass<UStaticMeshComponent>();
	// 	if (MyStaticMesh)
	// 	{
	// 		UE_LOG(LogTemp, Log, TEXT("B 成功获取蓝图中的StaticMesh: %s"), *MyStaticMesh->GetName());
	// 	}
	// }

	{
		MyStaticMesh = Cast<UStaticMeshComponent>(CollisionComp->GetChildComponent(0));

		// if (MyStaticMesh)
		// {
		// 	UE_LOG(LogTemp, Log, TEXT("C 成功获取蓝图中的StaticMesh: %s"), *MyStaticMesh->GetName());
		// }
	}
	return MyStaticMesh;
}

void AProjectileActor::RandomChangeProjectileColor()
{
	// ==============================================
	// 1. 获取 StaticMesh 组件
	// ==============================================
	UStaticMeshComponent* MyStaticMesh = GetStaticMeshComp();
	if (!IsValid(MyStaticMesh)) return;

	// ==============================================
	// 2. 获取/创建动态材质实例
	// ==============================================
	UMaterialInterface* Material = MyStaticMesh->GetMaterial(0);
	if (!IsValid(Material)) return;

	UMaterialInstanceDynamic* DynamicMat = nullptr;
	if (Material->IsA(UMaterialInstanceDynamic::StaticClass()))
	{
		DynamicMat = Cast<UMaterialInstanceDynamic>(Material);
	}
	else
	{
		DynamicMat = UMaterialInstanceDynamic::Create(Material, this);
		MyStaticMesh->SetMaterial(0, DynamicMat);
	}

	if (!IsValid(DynamicMat)) return;

	// ==============================================
	// 3. 随机变色（核心）
	// ==============================================
	const float R = FMath::RandRange(0.0f, 1.0f);
	const float G = FMath::RandRange(0.0f, 1.0f);
	const float B = FMath::RandRange(0.0f, 1.0f);
	FLinearColor RandomColor(R, G, B, 1.0f);

	DynamicMat->SetVectorParameterValue(FName("DiffuseColor"), RandomColor);
	DynamicMat->SetScalarParameterValue(FName("Metallic"), FMath::RandRange(0.0f, 1.0f));
	DynamicMat->SetScalarParameterValue(FName("Roughness"), FMath::RandRange(0.0f, 1.0f));

	// UE_LOG(LogTemp, Log, TEXT("子弹已随机变色！"));
}
