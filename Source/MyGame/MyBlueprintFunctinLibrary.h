// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctinLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyBlueprintFunctinLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void Test(UWorld* Obj);
	UFUNCTION(BlueprintCallable)
	static void ShowPopWindow();
	UFUNCTION(BlueprintCallable)
	static void TestMyMenuAnchor(UWorld* Obj);
	
	// 静态缓存：保存全局唯一的 Slate 控件（关键）
	static TMap<FName, TWeakPtr<SWidget>> WidgetCache;
};
