#include "MultiplayerLogHelper.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/NetConnection.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

// 实现自定义日志类别
DEFINE_LOG_CATEGORY(LogMultiplayer);

FString FMultiplayerLogHelper::GetFormattedTimestamp()
{
    FDateTime CurrentTime = FDateTime::UtcNow();
    return FString::Printf(TEXT("[%04d-%02d-%02d %02d:%02d:%02d.%03d]"),
        CurrentTime.GetYear(), CurrentTime.GetMonth(), CurrentTime.GetDay(),
        CurrentTime.GetHour(), CurrentTime.GetMinute(), CurrentTime.GetSecond(),
        CurrentTime.GetMillisecond()
    );
}

FString FMultiplayerLogHelper::GetPlayerUniqueID(UObject* PlayerContext)
{
    if (!PlayerContext)
    {
        return TEXT("未知玩家");
    }

    // 从PlayerState获取PlayerId（最推荐）
    APlayerState* PlayerState = nullptr;
    if (PlayerContext->IsA<APlayerState>())
    {
        PlayerState = Cast<APlayerState>(PlayerContext);
    }
    else if (PlayerContext->IsA<APlayerController>())
    {
        PlayerState = Cast<APlayerController>(PlayerContext)->PlayerState;
    }

    if (PlayerState && PlayerState->IsValidLowLevel())
    {
        // PlayerId是UE4分配的全局唯一整数ID，格式化为Client_01/Client_02等
        return FString::Printf(TEXT("Client_%02d"), PlayerState->GetPlayerId());
    }

    return TEXT("未登录玩家");
}

FString FMultiplayerLogHelper::GetClientNetworkInfo(UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return TEXT("未知IP");
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World || !World->IsClient())
    {
        return TEXT("非客户端");
    }

    // 获取客户端的网络连接信息（IP+端口）
    APlayerController* LocalPC = World->GetFirstPlayerController();
    if (LocalPC && LocalPC->NetConnection)
    {
        FString NetAddr = LocalPC->NetConnection->RemoteAddressToString();
        // 简化地址格式（去掉多余前缀）
        NetAddr = NetAddr.Replace(TEXT("0.0.0.0:"), TEXT(""));
        NetAddr = NetAddr.Replace(TEXT(":::"), TEXT(""));
        return FString::Printf(TEXT("IP_%s"), *NetAddr);
    }

    return TEXT("未知IP");
}

FString FMultiplayerLogHelper::GetNetworkIdentityWithID(UObject* WorldContextObject, UObject* PlayerContext)
{
    if (!WorldContextObject)
    {
        return TEXT("[未知环境]");
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        return TEXT("[无世界上下文]");
    }

    // 判断网络身份 + 拼接唯一标识
    if (World->IsServer() && World->IsNetMode(NM_ListenServer))
    {
        // 主机（ListenServer）
        return TEXT("[主机/Host]");
    }
    else if (World->IsServer())
    {
        // 专用服务器
        return TEXT("[专用服务器/Server]");
    }
    else if (World->IsClient())
    {
        // 客户端：优先用PlayerID，备用用IP+端口
        FString ClientID = GetPlayerUniqueID(PlayerContext);
        if (ClientID == TEXT("未知玩家") || ClientID == TEXT("未登录玩家"))
        {
            ClientID = GetClientNetworkInfo(WorldContextObject);
        }
        return FString::Printf(TEXT("[客户端/%s]"), *ClientID);
    }
    else
    {
        // 单机模式
        return TEXT("[单机/Standalone]");
    }
}

void FMultiplayerLogHelper::LogWithIdentity(const FString& LogContent, ELogLevel Level, 
                                            UObject* WorldContextObject, UObject* PlayerContext)
{
    // 拼接完整日志：时间戳 + 带ID的网络身份 + 日志内容
    FString FullLog = GetFormattedTimestamp() + GetNetworkIdentityWithID(WorldContextObject, PlayerContext) + TEXT(" ") + LogContent;

    // 按级别输出
    switch (Level)
    {
        case ELogLevel::Log:
            UE_LOG(LogMultiplayer, Log, TEXT("%s"), *FullLog);
            break;
        case ELogLevel::Warning:
            UE_LOG(LogMultiplayer, Warning, TEXT("%s"), *FullLog);
            break;
        case ELogLevel::Error:
            UE_LOG(LogMultiplayer, Error, TEXT("%s"), *FullLog);
            break;
        case ELogLevel::Fatal:
            UE_LOG(LogMultiplayer, Fatal, TEXT("%s"), *FullLog);
            break;
        default:
            UE_LOG(LogMultiplayer, Log, TEXT("%s"), *FullLog);
            break;
    }
}