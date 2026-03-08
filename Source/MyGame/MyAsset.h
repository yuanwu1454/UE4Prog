// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MYGAME_API UMyAsset : public UObject
{
	GENERATED_BODY()
public:
	// ----------------------
	// 底层：非UPROPERTY数据（序列化到UAsset，但蓝图不可见）
	// ----------------------
	TMap<FString, int32> CustomMap; // 非UPROPERTY，仅通过Serialize序列化

	// ----------------------
	// 上层：UPROPERTY/UFUNCTION包装层（蓝图可见，间接操作CustomMap）
	// ----------------------
	// 1. 蓝图可编辑的「临时属性」：用于传入操作参数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CustomMap")
	FString MapKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CustomMap")
	int32 MapValue;

	// 2. 蓝图可调用的方法：操作底层非UPROPERTY数据
	UFUNCTION(BlueprintCallable, Category="CustomMap")
	void AddToCustomMap()
	{
		CustomMap.Add(MapKey, MapValue); // 间接修改非UPROPERTY数据
	}

	UFUNCTION(BlueprintCallable, Category="CustomMap")
	int32 GetFromCustomMap(const FString& Key)
	{
		return CustomMap.Contains(Key) ? CustomMap[Key] : -1; // 间接读取
	}

	UFUNCTION(BlueprintCallable, Category="CustomMap")
	TArray<FString> GetAllMapKeys()
	{
		TArray<FString> Keys;
		CustomMap.GetKeys(Keys);
		return Keys; // 把非UPROPERTY数据转为蓝图可见的数组
	}

	// ----------------------
	// 序列化：保存非UPROPERTY数据到UAsset
	// ----------------------
	virtual void Serialize(FArchive& Ar) override
	{
		Super::Serialize(Ar);
		Ar << CustomMap; // 非UPROPERTY数据序列化到UAsset
	}

public:
	// 静态测试函数：验证C++层面的序列化+UAsset保存/加载
	UFUNCTION(BlueprintCallable, Category="CustomMap|Test")
	static void TestCustomMapSerialization();

	UFUNCTION(BlueprintCallable, Category="CustomMap|UAsset")
	static UMyAsset* CreateAndSaveAsUAsset(const FString& AssetPath);

	UFUNCTION(BlueprintCallable, Category="CustomMap|UAsset")
	void InitTestData();
};