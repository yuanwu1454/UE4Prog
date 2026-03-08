// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyGameGameModeBase.h"
#include "MyHUD.h"
#include "MyPlayerCharacter.h"
#include "GameFramework/PlayerController.h"


AMyGameGameModeBase::AMyGameGameModeBase()
{
	// 核心：将HUDClass赋值为你的自定义HUD类
	HUDClass = AMyHUD::StaticClass();
}

void AMyGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();
    // 直接调用初始化函数，兼容单机/服务器
    InitPlayerAttributes(800.0f, 144);
}
// 场景2：服务器模式（客户端连接后自动初始化）
void AMyGameGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // 客户端连接后，主动触发初始化（覆盖重试逻辑，优先执行）
    InitPlayerAttributes(800.0f, 144);
}

// 核心：兼容单机/服务器的玩家获取逻辑
AMyPlayerCharacter* AMyGameGameModeBase::GetValidPlayerCharacter(bool bForceLocal)
{
    // ========== 第一步：区分「权限身份」 ==========
    bool bIsServer = HasAuthority();
    bool bIsLocalClient = GetWorld()->IsNetMode(NM_Standalone) || GetWorld()->IsNetMode(NM_Client);

    // ========== 第二步：分模式获取玩家 ==========
    APlayerController* TargetPC = nullptr;
    if (bForceLocal || (bIsLocalClient && !bIsServer))
    {
        // 纯客户端/单机模式：获取本地 PlayerController
        TargetPC = GetWorld()->GetFirstPlayerController();
    }
    else if (bIsServer)
    {
        // 服务器模式（含单机的Listen Server）：获取已连接的第一个玩家
        TargetPC = GetWorld()->GetFirstPlayerController();
        
        // 服务器模式下，若暂无玩家，返回空（后续重试）
        if (!TargetPC)
        {
            UE_LOG(LogTemp, Warning, TEXT("服务器模式：暂无客户端连接，PlayerController为空"));
            return nullptr;
        }
    }

    // ========== 第三步：安全获取 Pawn ==========
    if (!TargetPC)
    {
        UE_LOG(LogTemp, Warning, TEXT("未找到有效 PlayerController"));
        return nullptr;
    }

    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(TargetPC->GetPawn());
    if (!Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerController存在，但Pawn尚未生成"));
        return nullptr;
    }

    // 重置重试计数（获取成功）
    CurrentRetryCount = 0;
    return Player;
}

// 示例：初始化玩家属性（兼容单机/服务器）
void AMyGameGameModeBase::InitPlayerAttributes(float NewSpeed, int32 NewFPS)
{
    // 第一步：尝试获取玩家
    AMyPlayerCharacter* Player = GetValidPlayerCharacter();
    if (Player)
    {
        // 第二步：获取成功，执行逻辑（服务器修改会同步，单机直接生效）
        Player->UpdateAttributes(NewSpeed, NewFPS);
        UE_LOG(LogTemp, Log, TEXT("玩家属性初始化成功：Speed=%.1f, FPS=%d"), NewSpeed, NewFPS);
        return;
    }

    // 第三步：获取失败（服务器模式暂无玩家），延迟重试（兼容服务器模式）
    if (HasAuthority() && CurrentRetryCount < MaxRetryCount)
    {
        CurrentRetryCount++;
        UE_LOG(LogTemp, Log, TEXT("重试获取玩家（第%d次）"), CurrentRetryCount);
        GetWorld()->GetTimerManager().SetTimer(RetryGetPlayerTimer, FTimerDelegate::CreateLambda([this, NewSpeed, NewFPS]()
        {
            InitPlayerAttributes(NewSpeed, NewFPS); // 递归重试
        }), 0.5f, false);
    }
    else if (CurrentRetryCount >= MaxRetryCount)
    {
        // 重试上限，兜底处理
        UE_LOG(LogTemp, Error, TEXT("多次重试仍未获取到玩家，放弃初始化"));
        CurrentRetryCount = 0; // 重置计数
    }
}