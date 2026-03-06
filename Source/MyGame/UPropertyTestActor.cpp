// UPropertyTestActor.cpp
#include "UPropertyTestActor.h"

#include "Net/UnrealNetwork.h"

AUPropertyTestActor::AUPropertyTestActor()
{
	PrimaryActorTick.bCanEverTick = false;
	// 设置根组件（避免Actor在编辑器中无可视化）
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));


	// 验证属性是否被反射识别
	const UClass* Class = GetClass();
	const FFloatProperty* Prop = FindField<FFloatProperty>(Class, TEXT("CallbackFloat"));
	if (Prop)
	{
		UE_LOG(LogTemp, Log, TEXT("✅ 属性CallbackFloat反射识别成功"));
		// 验证OnValueChanged元数据是否存在
		FString OnChangedName= Prop->GetMetaData(FName(TEXT("OnValueChangedx")));
		if (!OnChangedName.IsEmpty())
		{
			UE_LOG(LogTemp, Log, TEXT("✅ 回调绑定成功: %s"), *OnChangedName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ 未找到OnValueChanged元数据"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ 属性CallbackFloat反射识别失败"));
	}
}

void AUPropertyTestActor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("UPropertyTestActor BeginPlay: CallbackFloat当前值 = %f"), CallbackFloat);
}

// 回调函数实现
void AUPropertyTestActor::OnTestValueChanged(float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("CallbackFloat被修改为: %f"), NewValue);
}

void AUPropertyTestActor::SetCallbackFloatValue(float NewValue)
{
	// 直接修改属性值
	CallbackFloat = NewValue;
}

// 注册复制属性（Replicated修饰符必须）
void AUPropertyTestActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 注册ReplicatedFloat为可复制属性
	DOREPLIFETIME(AUPropertyTestActor, ReplicatedFloat);
}

void AUPropertyTestActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property == nullptr) return;

	FName PropName = PropertyChangedEvent.Property->GetFName();
	// 监听CallbackFloat
	if (PropName == TEXT("CallbackFloat"))
	{
		OnTestValueChanged(CallbackFloat);
	}
	// 监听其他属性（比如CallbackInt）
	// else if (PropName == TEXT("CallbackInt"))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("CallbackInt被修改为: %d"), CallbackInt);
	// }
}