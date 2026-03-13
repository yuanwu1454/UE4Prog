// ControllableActor.cpp
#include "ControllableActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AControllableActor::AControllableActor()
{
	// 设置根组件
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComponent;

	// 创建网格体组件
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);

	// 设置默认网格体（可以在蓝图中替换）
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (DefaultMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(DefaultMesh.Object);
	}

	// 启用Tick（可选，根据需求）
	PrimaryActorTick.bCanEverTick = true;
}

void AControllableActor::BeginPlay()
{
	Super::BeginPlay();
}

void AControllableActor::MoveActor(const FVector& Direction, float Speed)
{
	if (Direction.IsNearlyZero()) return;

	// 计算移动增量
	FVector MoveDelta = Direction.GetSafeNormal() * Speed * GetWorld()->GetDeltaSeconds();
	// 移动Actor
	AddActorWorldOffset(MoveDelta, true);
}

void AControllableActor::RotateActor(float YawDelta, float PitchDelta, float RollDelta, float RotationSpeed)
{
	if (FMath::IsNearlyZero(YawDelta) && FMath::IsNearlyZero(PitchDelta) && FMath::IsNearlyZero(RollDelta))
		return;

	// 计算旋转增量（欧拉角转四元数）
	FRotator RotDelta(YawDelta * RotationSpeed * GetWorld()->GetDeltaSeconds(),
					  PitchDelta * RotationSpeed * GetWorld()->GetDeltaSeconds(),
					  RollDelta * RotationSpeed * GetWorld()->GetDeltaSeconds());
    
	// 旋转Actor
	AddActorWorldRotation(RotDelta.Quaternion(), true);
}