// Fill out your copyright notice in the Description page of Project Settings.

// UPropertyTestActor.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UPropertyTestActor.generated.h"

// 测试用的枚举（用于验证Enum类型的UPROPERTY）
UENUM(BlueprintType)
enum class ETestEnum : uint8
{
    Test_1 UMETA(DisplayName="测试选项1"),
    Test_2 UMETA(DisplayName="测试选项2"),
    Test_3 UMETA(DisplayName="测试选项3")
};

UCLASS()
class MYGAME_API AUPropertyTestActor : public AActor
{
    GENERATED_BODY()
    
public:
    AUPropertyTestActor();

    // ======================================
    // 1. 编辑器可编辑类修饰符测试
    // ======================================
    // EditAnywhere: 所有地方（默认值/实例）都能编辑
    UPROPERTY(EditAnywhere, Category="1.编辑器编辑测试")
    float EditAnywhereFloat = 100.0f;

    // EditDefaultsOnly: 仅蓝图默认值能编辑，关卡实例不能改
    UPROPERTY(EditDefaultsOnly, Category="1.编辑器编辑测试")
    int32 EditDefaultsOnlyInt = 50;

    // EditInstanceOnly: 仅关卡实例能编辑，蓝图默认值不能改
    UPROPERTY(EditInstanceOnly, Category="1.编辑器编辑测试")
    FString EditInstanceOnlyStr = TEXT("实例专属文本");

    // VisibleAnywhere: 所有地方可见，但不能编辑
    UPROPERTY(VisibleAnywhere, Category="1.编辑器编辑测试")
    bool VisibleAnywhereBool = true;

    // ======================================
    // 2. 蓝图访问类修饰符测试
    // ======================================
    // BlueprintReadWrite: 蓝图可读可写
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="2.蓝图访问测试")
    ETestEnum BlueprintReadWriteEnum = ETestEnum::Test_1;

    // BlueprintReadOnly: 蓝图只读，不能写
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="2.蓝图访问测试")
    FVector BlueprintReadOnlyVec = FVector(100, 200, 300);

    // ======================================
    // 3. Meta元数据修饰符测试
    // ======================================
    // Tooltip: 鼠标悬浮显示提示文本
    UPROPERTY(EditAnywhere, Category="3.Meta元数据测试", meta=(Tooltip="这是带提示的浮点数，范围0-100"))
    float MetaTooltipFloat = 50.0f;

    // ClampMin/ClampMax: 限制数值范围
    UPROPERTY(EditAnywhere, Category="3.Meta元数据测试", meta=(ClampMin=0, ClampMax=100))
    int32 MetaClampInt = 50;

    // EditCondition: 条件编辑（只有bShowMetaEditCond为true时，该属性才显示可编辑）
    UPROPERTY(EditAnywhere, Category="3.Meta元数据测试")
    bool bShowMetaEditCond = false;

    UPROPERTY(EditAnywhere, Category="3.Meta元数据测试", meta=(EditCondition="bShowMetaEditCond"))
    FString MetaEditConditionStr = TEXT("条件显示的文本");

    // UIMin/UIMax: 编辑器滑块范围（不限制实际值，仅视觉滑块）
    UPROPERTY(EditAnywhere, Category="3.Meta元数据测试", meta=(UIMin=0, UIMax=10))
    float MetaUISliderFloat = 5.0f;

    // ======================================
    // 4. 序列化/复制测试（需结合关卡保存/联网验证）
    // ======================================
    // SaveGame: 标记为可保存到存档（序列化）
    UPROPERTY(EditAnywhere, Category="4.序列化/复制测试", meta=(SaveGame))
    FString SaveGameStr = TEXT("存档测试文本");

    // Replicated: 联网复制（服务端同步到客户端）
    // 注意：需在类中重写GetLifetimeReplicatedProps()注册复制属性
    UPROPERTY(EditAnywhere, Category="4.序列化/复制测试", Replicated)
    float ReplicatedFloat = 0.0f;

    // ======================================
    // 5. 回调测试（属性修改触发函数）
    // ======================================
    // OnValueChanged: 属性修改时触发指定函数 不存在
    UPROPERTY(EditAnywhere, Category="5.回调测试", meta=(OnValueChanged="OnTestValueChanged"))
    float CallbackFloat = 0.0f;

    // 回调函数（必须是UFUNCTION，且参数匹配属性类型）
    UFUNCTION()
    void OnTestValueChanged(float NewValue);

    
    UFUNCTION(BlueprintCallable, Category="5.回调测试")
    void SetCallbackFloatValue(float NewValue);

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
protected:
    virtual void BeginPlay() override;

    // 注册复制属性（用于Replicated修饰符测试）
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};