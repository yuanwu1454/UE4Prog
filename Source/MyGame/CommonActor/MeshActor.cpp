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


	
	// 游戏开始自动修改颜色（可注释掉，按需调用）
	// if (SkeletalMeshComp)
	// {
	// 	UpdateMaterialColor(MaterialSlotIndex, TargetColor);
	// }
	
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
	// if (!SkeletalMeshComp) return;
	//
	// // 获取材质总数量
	// const int32 MaterialNum = SkeletalMeshComp->GetMaterials().Num();
	// if (MaterialNum == 0) return;
	//
	// // 确定要修改的插槽索引范围
	// TArray<int32> TargetSlotIndices;
	// if (InSlotIndex >= 0 && InSlotIndex < MaterialNum)
	// {
	// 	TargetSlotIndices.Add(InSlotIndex);
	// }
	// else
	// {
	// 	// 索引无效时，修改所有插槽
	// 	for (int32 i = 0; i < MaterialNum; i++)
	// 	{
	// 		TargetSlotIndices.Add(i);
	// 	}
	// }
	//
	// // 遍历修改每个插槽
	// for (int32 SlotIdx : TargetSlotIndices)
	// {
	// 	UMaterialInterface* OriginalMaterial = SkeletalMeshComp->GetMaterial(SlotIdx);
	// 	if (!OriginalMaterial) continue;
	//
	// 	// 1. 判断是否为材质实例（非基础材质）
	// 	UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(OriginalMaterial);
	// 	if (!MaterialInstance)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("插槽%d不是材质实例，跳过修改"), SlotIdx);
	// 		continue;
	// 	}
	//
	// 	// 2. 创建动态材质实例（运行时可修改）
	// 	UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(MaterialInstance, this);
	// 	if (!DynamicMat)
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("创建动态材质实例失败，插槽%d"), SlotIdx);
	// 		continue;
	// 	}
	// 	// 3. 设置颜色参数（对应你截图中的BodyColor）
	//
	// 	DynamicMat->SetVectorParameterValue(ColorParameterName, InColor);
	// 	// 4. 应用到网格体
	// 	SkeletalMeshComp->SetMaterial(SlotIdx, DynamicMat);
	// }
    
    	for (int32 i = 0; i < CachedMIDs.Num(); i++)
    	{
    		if (CachedMIDs[i])
    		{
    			CachedMIDs[i]->SetVectorParameterValue("BodyColor", RandomColor);
    		}
    	}
}