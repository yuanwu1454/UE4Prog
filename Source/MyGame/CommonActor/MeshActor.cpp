// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshActor.h"

#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"


// Sets default values
AMeshActor::AMeshActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// 1. 创建根组件
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	// 2. 创建骨骼网格体组件
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(RootScene);

	// 初始化索引
	CurrentMeshIndex = 0;
}

// Called when the game starts or when spawned
void AMeshActor::BeginPlay()
{
	Super::BeginPlay();


	if (bChangeMesh)
	{
		// 开始每 1 秒切换一次 Mesh
		if (MeshList.Num() > 0)
		{
			GetWorldTimerManager().SetTimer(
				TimerHandle_SwitchMesh,   // 定时器句柄
				this,                     // 当前对象
				&AMeshActor::SwitchToNextMesh, // 要执行的函数
				1.0f,                     // 间隔秒数
				true                      // 循环执行
			);
		}
	}

	if (bChangeColor)
	{
		
		// 预先创建并缓存所有MID（只执行1次）
		for (int32 i = 0; i < SkeletalMeshComp->GetNumMaterials(); i++)
		{
			UMaterialInterface* Mat = SkeletalMeshComp->GetMaterial(i);
			UMaterialInstance* MI = Cast<UMaterialInstance>(Mat);
			if (MI)
			{
				UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(MI, this);
				SkeletalMeshComp->SetMaterial(i, MID);
				CachedMIDs.Add(MID);
			}
			else
			{
				CachedMIDs.Add(nullptr);
			}
		}
	

		// 开始每 1 秒切换一次 Mesh
		GetWorldTimerManager().SetTimer(
			TimerHandle_SwitchColor,   // 定时器句柄
			[this]()
			{
				UpdateMaterialColor(1, FColor::MakeRandomColor());
			},
			1.0f,                     // 间隔秒数
			true                      // 循环执行
		);
	}
}

// ==============================================
// 核心：每隔1秒切换下一个Mesh
// ==============================================
void AMeshActor::SwitchToNextMesh()
{
	if (!SkeletalMeshComp || MeshList.Num() == 0)
		return;

	// 切换模型
	SkeletalMeshComp->SetSkeletalMesh(MeshList[CurrentMeshIndex]);

	// 索引+1，循环
	CurrentMeshIndex++;
	if (CurrentMeshIndex >= MeshList.Num())
	{
		CurrentMeshIndex = 0;
	}
}


// ==============================================
// 核心：修改材质颜色逻辑
// ==============================================
void AMeshActor::UpdateMaterialColor(int32 InSlotIndex, FLinearColor RandomColor)
{
    for (int32 i = 0; i < CachedMIDs.Num(); i++)
    {
    	if (CachedMIDs[i])
    	{
    		CachedMIDs[i]->SetVectorParameterValue("BodyColor", RandomColor);
    	}
    }
}