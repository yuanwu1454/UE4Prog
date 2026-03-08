// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Config=Game)
class MYGAME_API UMyObject : public UObject
{
	GENERATED_BODY()
public:
	UMyObject();

	// ========== 1. 反射（UProperty/UFunction） ==========
	// 可序列化、可反射、可在蓝图中访问的属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyObject|Basic", SaveGame, Config)
	FString MyString = TEXT("默认字符串");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyObject|Basic", SaveGame)
	int32 MyInt = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyObject|Basic", SaveGame)
	float MyFloat = 3.14f;

	// 可反射、可在蓝图中调用的函数
	UFUNCTION(BlueprintCallable, Category="MyObject|Function")
	void PrintObjectInfo();

	// ========== 2. 序列化（自定义序列化逻辑） ==========
	virtual void Serialize(FArchive& Ar) override;

	// ========== 3. 配置文件读写（Config） ==========
	// 手动触发配置文件保存
	UFUNCTION(BlueprintCallable, Category="MyObject|Config")
	void SaveToConfig();

	// 手动触发从配置文件加载
	UFUNCTION(BlueprintCallable, Category="MyObject|Config")
	void LoadFromConfig();
	
	// 重写BeginDestroy，在对象销毁前执行自定义逻辑
	virtual void BeginDestroy() override;

	void SaveToFile(const FString& FilePath);
	void LoadFromFile(const FString& FilePath);

	// ========== 5. 网络同步（Replication） ==========
	// 注：UObject默认不支持网络同步，需手动标记Replicate
	// 若需网络同步，建议继承AActor或UActorComponent，此处仅演示UObject的同步声明方式
	virtual bool IsSupportedForNetworking() const override { return true; }

	// 序列化时的自定义数据（演示非UPROPERTY的序列化）
	TArray<FName> CustomData;
	static void Test();
};
