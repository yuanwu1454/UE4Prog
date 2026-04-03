// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyStringTableAsset.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyStringTableAsset : public UDataAsset
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<class UStringTable*>					StringTables;
};
// 蓝图里弱引用 / 强引用 这个 UDataAsset（BP_StringTableAsset）到底要不要手动加载？
// ✔ 强引用 = 自动加载，不用管
// ✔ 弱引用 = 必须手动加载（RequestAsyncLoad / LoadSynchronous）


