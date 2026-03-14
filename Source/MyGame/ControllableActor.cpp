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