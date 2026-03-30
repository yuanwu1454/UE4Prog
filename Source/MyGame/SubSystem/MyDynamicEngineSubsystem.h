#pragma once
#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "MyDynamicEngineSubsystem.generated.h"

UCLASS()
class MYGAME_API UMyDynamicEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	// 生命周期
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// 测试方法
	UFUNCTION(BlueprintCallable, Category = "Dynamic Subsystem")
	void TestDynamicSubsystem();
};