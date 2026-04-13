// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelTrigger.generated.h"

UCLASS()
class MYGAME_API ALevelTrigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere, Category="Trigger")
	class UBoxComponent* TriggerVolume;

	// 通用配置：每个实例改这个就行
	UPROPERTY(EditAnywhere, Category="Level")
	FName TargetLevel;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	// 把原来的TargetLevel改成子关卡名，新增一个是否只加载一次的开关
	UPROPERTY(EditAnywhere, Category="Level")
	FName SubLevelToLoad;

	UPROPERTY(EditAnywhere, Category="Level")
	bool bLoadOnce = true; // 防止重复加载

	// 新增加载完成的回调
	UFUNCTION()
	void OnSubLevelLoaded();
	UFUNCTION()
	void OnSubLevelShown();

	
	UPROPERTY(EditAnywhere, Category="Level")
	bool bLoadMainLevel;

private:
	bool bHasLoaded = false; // 标记是否已经加载过
};
