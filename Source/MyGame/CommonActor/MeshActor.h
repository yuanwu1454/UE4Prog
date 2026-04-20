// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorBase.h"
#include "MeshActor.generated.h"

UCLASS()
class MYGAME_API AMeshActor : public AActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// ==============================================
	// 组件声明（会自动生成）
	// ==============================================
	// 根组件
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* RootScene;

	// 骨骼网格体（带动画）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* SkeletalMeshComp;

	
	// ==============================================
	// 【新增】循环切换 Mesh 功能
	// ==============================================
	// 在这里添加你要切换的多个人物模型
	UPROPERTY(EditAnywhere, Category = "Mesh Switch")
	TArray<USkeletalMesh*> MeshList;

	// 当前显示到第几个
	int32 CurrentMeshIndex;

	// 定时器句柄
	FTimerHandle TimerHandle_SwitchMesh;
	// 定时器句柄
	FTimerHandle TimerHandle_SwitchColor;

	// 切换 Mesh 的函数
	UFUNCTION()
	void SwitchToNextMesh();

	// ==============================================
	// 可编辑配置参数
	// ==============================================
	// 要修改的颜色参数名称（对应你截图中的BodyColor）
	UPROPERTY(EditAnywhere, Category = "Material Setup")
	FName ColorParameterName = "BodyColor";

	// 目标颜色（可在编辑器中直接调色）
	UPROPERTY(EditAnywhere, Category = "Material Setup", meta = (HideAlphaChannel))
	FLinearColor TargetColor = FLinearColor::White;

	// 要修改的材质插槽索引（-1表示修改所有插槽）
	UPROPERTY(EditAnywhere, Category = "Material Setup")
	int32 MaterialSlotIndex = 1;

	// ==============================================
	// 核心功能函数
	// ==============================================
	/**
	 * 修改材质颜色
	 * @param InSlotIndex 材质插槽索引（-1修改所有）
	 * @param InColor 目标颜色
	 */
	UFUNCTION(BlueprintCallable, Category = "Material")
	void UpdateMaterialColor(int32 InSlotIndex = -1, FLinearColor InColor = FLinearColor::White);

	// 要修改的材质插槽索引（-1表示修改所有插槽）
	UPROPERTY(EditAnywhere, Category = "Material Setup")
	bool bChangeColor = false;
	// 要修改的材质插槽索引（-1表示修改所有插槽）
	UPROPERTY(EditAnywhere, Category = "Material Setup")
	bool bChangeMesh = false;

	// 缓存动态材质
	UPROPERTY(Transient)
	TArray<UMaterialInstanceDynamic*> CachedMIDs;
};
