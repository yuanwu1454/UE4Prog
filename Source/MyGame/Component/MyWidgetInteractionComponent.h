// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetInteractionComponent.h"
#include "MyWidgetInteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYGAME_API UMyWidgetInteractionComponent : public UWidgetInteractionComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMyWidgetInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};


/***
UWidgetInteractionComponent 是 UE 官方提供的3D 世界 → 2D UI 交互转换器，
通过射线检测定位 Widget，模拟虚拟鼠标 / 键盘，让玩家能像点屏幕一样点 3D 空间里的 UI。
重写对应的方法，或者调用对应的方法，监听对应的代理 来实现对应的方法。
***/