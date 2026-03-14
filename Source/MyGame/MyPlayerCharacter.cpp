// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerCharacter.h"

#include "Log/MultiplayerLogHelper.h"
#include "Net/UnrealNetwork.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
	// 启用网络复制
	bReplicates = true;
	bAlwaysRelevant = true;
}

// 第一步：声明要同步的属性（仅MoveSpeed）
void AMyPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 仅注册MoveSpeed为同步属性，TempFPS不注册
	DOREPLIFETIME(AMyPlayerCharacter, MoveSpeed);
}

// 第二步：重写Serialize，仅序列化需要同步的属性
void AMyPlayerCharacter::Serialize(FArchive& Ar)
{
	// 判断：仅在「网络同步归档」时优化，其他场景（如保存）仍调用父类
	if (Ar.IsNetArchive()) // 关键：只优化网络同步的序列化
		{
		// 仅序列化需要同步的核心属性（MoveSpeed）
		// 通过相关状态 来区分指定情况下 是否同步指定属性，这样可以减轻数据带宽
		Ar << MoveSpeed;
		UE_LOG(LogTemp, Log, TEXT("网络同步：仅序列化MoveSpeed=%.1f"), MoveSpeed);
		}
	else
	{
		// 非网络场景（如保存UAsset/本地序列化），正常调用父类
		Super::Serialize(Ar);
	}
}

// 测试方法：修改属性（模拟游戏中实时变化）
void AMyPlayerCharacter::UpdateAttributes(float NewSpeed, int32 NewFPS)
{
	if (HasAuthority()) // 单机/服务器才允许修改
	{
		MoveSpeed = NewSpeed;
		TempFPS = NewFPS;
	}
	else
	{
		// 客户端请求服务器修改（通过RPC）
		Server_UpdateAttributes(NewSpeed, NewFPS);
	}
}

void AMyPlayerCharacter::UpdateMoveSpeed(float NewSpeed)
{
	if (HasAuthority()) // 单机/服务器才允许修改
		{
		MoveSpeed = NewSpeed;
		}
	else
	{
		// 客户端请求服务器修改（通过RPC）
		Server_UpdateMoveSpeed(NewSpeed);
	}
}

void AMyPlayerCharacter::Server_UpdateMoveSpeed_Implementation(float NewSpeed)
{
	MoveSpeed = NewSpeed;
}


// ------------ 规则2：Server RPC 的实现函数（必写，后缀 _Implementation） ------------
// 作用：服务器端实际执行的逻辑（修改属性、同步数据）
void AMyPlayerCharacter::Server_UpdateAttributes_Implementation(float NewSpeed, int32 NewFPS)
{
	// 服务器端修改属性（会自动同步到客户端，因为 MoveSpeed 标记了 Replicated）
	MoveSpeed = NewSpeed;
	TempFPS = NewFPS;

	UE_LOG(LogTemp, Log, TEXT("服务器执行 RPC：修改 MoveSpeed=%.1f, TempFPS=%d"), MoveSpeed, TempFPS);
}

// ------------ 属性同步回调 ------------
void AMyPlayerCharacter::OnRep_MoveSpeed()
{
	UE_LOG(LogTemp, Log, TEXT("客户端同步 MoveSpeed：%.1f"), MoveSpeed);
}

void AMyPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 记录新客户端登录，传入PlayerController作为PlayerContext
	MULTI_LOG(FString::Printf(TEXT("PossessedBy：%s"), *this->GetName()), this, this);
}
