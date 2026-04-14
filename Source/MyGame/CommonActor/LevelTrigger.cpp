// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTrigger.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Called when the game starts or when spawned
void ALevelTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTrigger::OnOverlap);
}

// Called every frame
void ALevelTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


ALevelTrigger::ALevelTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	RootComponent = TriggerVolume;

	TriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
	TriggerVolume->SetGenerateOverlapEvents(true);
}

void ALevelTrigger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (bLoadMainLevel)
	{
		// 只响应玩家
		if (Cast<ACharacter>(OtherActor) && !TargetLevel.IsNone())
		{
			UGameplayStatics::OpenLevel(this, TargetLevel, false);
		}		
	}
	else{
		// 只响应玩家，且只加载一次
		if (Cast<ACharacter>(OtherActor) && !SubLevelToLoad.IsNone() && !(bLoadOnce && bHasLoaded))
		{
			bLoadOnce = true;
			// 关闭重叠，防止重复触发
			if (bLoadOnce)
			{
				TriggerVolume->SetGenerateOverlapEvents(false);
				bHasLoaded = true;
			}
			//

			// if (StreamLevel && !StreamLevel->IsLevelLoaded() && !StreamLevel->IsStreamingStatePending())
			// {
			// 	return;
			// }

			// 核心：加载子关卡，保留主地图！
			UGameplayStatics::LoadStreamLevel(
				this,                  // 世界上下文
				SubLevelToLoad,        // 子关卡名（直接填King_SubB）
				true,                  // 加载完成后显示
				false,                 // 不阻塞（异步加载）
				FLatentActionInfo()    // 回调（可选，这里用绑定的方式）
			);
			ULevelStreaming* StreamLevel = UGameplayStatics::GetStreamingLevel(this, SubLevelToLoad);
			if (StreamLevel)
			{
				StreamLevel->OnLevelLoaded.AddDynamic(this, &ALevelTrigger::OnSubLevelLoaded);
				StreamLevel->OnLevelShown.AddDynamic(this, &ALevelTrigger::OnSubLevelShown);
			}
		}
	}
}


// 加载完成的回调（可选，比如播放开门动画、隐藏提示）
void ALevelTrigger::OnSubLevelLoaded()
{
	UE_LOG(LogTemp, Log, TEXT("子关卡 %s 加载完成！"), *SubLevelToLoad.ToString());
}

// 加载完成的回调（可选，比如播放开门动画、隐藏提示）
void ALevelTrigger::OnSubLevelShown()
{
	UE_LOG(LogTemp, Log, TEXT("子关卡 %s 已经可视了！"), *SubLevelToLoad.ToString());
}

// LoadStreamLevel 子关卡
// IsStreamingStatePending()   // 正在加载中
// IsLevelLoaded()             // 已加载到内存
// IsLevelVisible()             // 已显示在世界
// 可以监听 

// UGameplayStatics::OpenLevel 主关卡