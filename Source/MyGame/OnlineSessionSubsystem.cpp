// OnlineSessionSubsystem.cpp
#include "OnlineSessionSubsystem.h"

#include "ProjectNetworkUtils.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


void UOnlineSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // 初始化OnlineSubsystem（全局唯一）
    OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        SessionInterface = OnlineSubsystem->GetSessionInterface();
        UE_LOG(LogOnlineSession, Log, TEXT("OnlineSessionSubsystem 初始化成功，使用子系统：%s"), *OnlineSubsystem->GetSubsystemName().ToString());
    }
    else
    {
        UE_LOG(LogOnlineSession, Error, TEXT("OnlineSessionSubsystem 初始化失败：未找到OnlineSubsystem"));
    }
}

bool UOnlineSessionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return UProjectNetworkUtils::IsServer(this);
}

bool UOnlineSessionSubsystem::CreateSession(FName SessionName, int32 MaxPlayers, bool bIsLAN)
{
    // 初始化SessionInterface
    if (!InitSessionInterface())
    {
        return false;
    }

    // 检查同名会话是否存在
    if (SessionInterface->GetNamedSession(SessionName) != nullptr)
    {
        UE_LOG(LogOnlineSession, Warning, TEXT("会话 %s 已存在，无需重复创建"), *SessionName.ToString());
        return false;
    }

    // 绑定创建完成回调
    SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
    SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnCreateSessionComplete)
    );

    // 构建会话设置
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = bIsLAN;
    SessionSettings.bUsesPresence = false;
    SessionSettings.NumPublicConnections = MaxPlayers;
    SessionSettings.bAllowJoinInProgress = true;
    SessionSettings.bAllowJoinViaPresence = true;
    SessionSettings.bShouldAdvertise = true;

    // 获取有效NetId
    FUniqueNetIdRepl NetId;
    if (!GetValidPlayerNetId(NetId, SessionName))
    {
        UE_LOG(LogOnlineSession, Error, TEXT("获取PlayerID失败，无法创建会话 %s"), *SessionName.ToString());
        return false;
    }

    // 异步创建会话
    bool bSuccess = SessionInterface->CreateSession(*NetId, SessionName, SessionSettings);
    if (!bSuccess)
    {
        UE_LOG(LogOnlineSession, Error, TEXT("同步调用CreateSession失败！会话 %s"), *SessionName.ToString());
        SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
        return false;
    }

    UE_LOG(LogOnlineSession, Log, TEXT("发起创建会话 %s 异步请求（最大玩家数：%d）"), *SessionName.ToString(), MaxPlayers);
    return true;
}

bool UOnlineSessionSubsystem::DestroySession(FName SessionName)
{
    if (!InitSessionInterface())
    {
        return false;
    }

    // 检查会话是否存在
    if (SessionInterface->GetNamedSession(SessionName) == nullptr)
    {
        UE_LOG(LogOnlineSession, Warning, TEXT("会话 %s 不存在，无需销毁"), *SessionName.ToString());
        return false;
    }

    // 绑定销毁完成回调
    SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
    SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
        FOnDestroySessionCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnDestroySessionComplete)
    );

    // 异步销毁会话
    bool bSuccess = SessionInterface->DestroySession(SessionName);
    if (!bSuccess)
    {
        UE_LOG(LogOnlineSession, Error, TEXT("同步调用DestroySession失败！会话 %s"), *SessionName.ToString());
        SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
        return false;
    }

    UE_LOG(LogOnlineSession, Log, TEXT("发起销毁会话 %s 异步请求"), *SessionName.ToString());
    return true;
}

bool UOnlineSessionSubsystem::FindAvailableSessions(bool bIsLAN, int32 MaxResults)
{
    if (!InitSessionInterface())
    {
        return false;
    }

    // 绑定查找完成回调
    SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
    SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnFindSessionsComplete)
    );

    // 构建查找配置
    SessionSearch = MakeShared<FOnlineSessionSearch>();
    SessionSearch->bIsLanQuery = bIsLAN;
    SessionSearch->MaxSearchResults = MaxResults;
    SessionSearch->PingBucketSize = 50;

    // 获取本地玩家NetId
    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
    {
        UE_LOG(LogOnlineSession, Error, TEXT("客户端PlayerID无效，无法查找会话"));
        return false;
    }

    // 异步查找会话
    bool bSuccess = SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
    if (!bSuccess)
    {
        UE_LOG(LogOnlineSession, Error, TEXT("同步调用FindSessions失败"));
        SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
        SessionSearch.Reset();
        return false;
    }

    UE_LOG(LogOnlineSession, Log, TEXT("发起查找会话异步请求（LAN：%s，最大结果数：%d）"), bIsLAN ? TEXT("是") : TEXT("否"), MaxResults);
    return true;
}

bool UOnlineSessionSubsystem::JoinGameSession(FName SessionName, const FOnlineSessionSearchResult& SessionResult)
{
    if (!InitSessionInterface() || !SessionResult.IsValid())
    {
        UE_LOG(LogOnlineSession, Error, TEXT("SessionInterface/会话结果无效，无法加入"));
        return false;
    }

    // 绑定加入完成回调
    SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
    SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &UOnlineSessionSubsystem::OnJoinSessionComplete)
    );

    // 获取本地玩家NetId
    ULocalPlayer* LocalPlayer =  GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
    {
        UE_LOG(LogOnlineSession, Error, TEXT("客户端PlayerID无效，无法加入会话"));
        return false;
    }

    // 异步加入会话
    bool bSuccess = SessionInterface->JoinSession(
        *LocalPlayer->GetPreferredUniqueNetId(),
        SessionName,
        SessionResult
    );

    if (!bSuccess)
    {
        UE_LOG(LogOnlineSession, Error, TEXT("同步调用JoinSession失败"));
        SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
        return false;
    }

    UE_LOG(LogOnlineSession, Log, TEXT("发起加入会话 %s 异步请求"), *SessionName.ToString());
    return true;
}

bool UOnlineSessionSubsystem::RegisterPlayerToSession(FName SessionName, const FUniqueNetIdRef& PlayerNetId)
{
    if (!InitSessionInterface())
    {
        return false;
    }

    bool bSuccess = SessionInterface->RegisterPlayer(SessionName, *PlayerNetId, false);
    if (bSuccess)
    {
        UE_LOG(LogOnlineSession, Log, TEXT("玩家 %s 注册到会话 %s 成功"), *PlayerNetId->ToString(), *SessionName.ToString());
    }
    else
    {
        UE_LOG(LogOnlineSession, Error, TEXT("玩家 %s 注册到会话 %s 失败"), *PlayerNetId->ToString(), *SessionName.ToString());
    }
    return bSuccess;
}

bool UOnlineSessionSubsystem::UnregisterPlayerFromSession(FName SessionName, const FUniqueNetIdRef& PlayerNetId)
{
    if (!InitSessionInterface())
    {
        return false;
    }

    bool bSuccess = SessionInterface->UnregisterPlayer(SessionName, *PlayerNetId);
    if (bSuccess)
    {
        UE_LOG(LogOnlineSession, Log, TEXT("玩家 %s 取消注册会话 %s 成功"), *PlayerNetId->ToString(), *SessionName.ToString());
    }
    else
    {
        UE_LOG(LogOnlineSession, Error, TEXT("玩家 %s 取消注册会话 %s 失败"), *PlayerNetId->ToString(), *SessionName.ToString());
    }
    return bSuccess;
}

// ===== 内部回调实现 =====
void UOnlineSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogOnlineSession, Log, TEXT("会话 %s 创建成功！"), *SessionName.ToString());
    }
    else
    {
        UE_LOG(LogOnlineSession, Error, TEXT("会话 %s 创建失败！"), *SessionName.ToString());
    }
    // 触发外部委托
    OnCreateSessionCompleteDelegate.ExecuteIfBound(SessionName, bWasSuccessful);
    SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
}

void UOnlineSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogOnlineSession, Log, TEXT("会话 %s 销毁成功！"), *SessionName.ToString());
    }
    else
    {
        UE_LOG(LogOnlineSession, Error, TEXT("会话 %s 销毁失败！"), *SessionName.ToString());
    }
    // 触发外部委托
    OnDestroySessionCompleteDelegate.ExecuteIfBound(SessionName, bWasSuccessful);
    SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
}

void UOnlineSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
    TArray<FOnlineSessionSearchResult> FoundSessions;
    if (bWasSuccessful && SessionSearch.IsValid())
    {
        FoundSessions = SessionSearch->SearchResults;
        UE_LOG(LogOnlineSession, Log, TEXT("查找会话完成，找到 %d 个可用会话"), FoundSessions.Num());
    }
    else
    {
        UE_LOG(LogOnlineSession, Error, TEXT("查找会话失败！"));
        SessionSearch.Reset();
    }
    // 触发外部委托
    // OnFindSessionsCompleteDelegate.Broadcast(FoundSessions);
    OnFindSessionsCompleteDelegate.ExecuteIfBound(FoundSessions);
    SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
}

void UOnlineSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    FString ResultStr;
    switch (Result)
    {
    case EOnJoinSessionCompleteResult::Success:
        {
            ResultStr = TEXT("成功");
            // 加入成功：跳转服务器地图
            FString ConnectString;
            if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
            {
            
                APlayerController* PC = GetWorld()->GetFirstPlayerController();
                if (PC)
                {
                    PC->ClientTravel(ConnectString, TRAVEL_Absolute);
                    UE_LOG(LogOnlineSession, Log, TEXT("客户端跳转至服务器：%s"), *ConnectString);
                }
            }
            break;            
        }
    case EOnJoinSessionCompleteResult::SessionDoesNotExist:
        {ResultStr = TEXT("会话不存在"); break;}
    case EOnJoinSessionCompleteResult::SessionIsFull:
        {ResultStr = TEXT("会话已满"); break;}
    case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
        {ResultStr = TEXT("无法获取服务器地址"); break;}
    default:
        {ResultStr = TEXT("未知错误"); break;}
    }

    UE_LOG(LogOnlineSession, Log, TEXT("加入会话 %s 结果：%s"), *SessionName.ToString(), *ResultStr);
    // 触发外部委托
    OnJoinSessionCompleteDelegate.Broadcast(SessionName, Result);
    SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
}

bool UOnlineSessionSubsystem::InitSessionInterface()
{
    if (!SessionInterface.IsValid())
    {
        if (!OnlineSubsystem)
        {
            OnlineSubsystem = IOnlineSubsystem::Get();
        }
        if (OnlineSubsystem)
        {
            SessionInterface = OnlineSubsystem->GetSessionInterface();
        }
        else
        {
            UE_LOG(LogOnlineSession, Error, TEXT("OnlineSubsystem 未找到"));
            return false;
        }
    }
    return SessionInterface.IsValid();
}

bool UOnlineSessionSubsystem::GetValidPlayerNetId(FUniqueNetIdRepl& OutNetId, FName SessionName)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogOnlineSession, Error, TEXT("无有效World，无法获取PlayerID"));
        return false;
    }

    // 客户端逻辑
    if (World->IsNetMode(NM_Client))
    {
        ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
        if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
        {
            UE_LOG(LogOnlineSession, Error, TEXT("[客户端] 本地PlayerID无效，会话 %s"), *SessionName.ToString());
            return false;
        }
        OutNetId = LocalPlayer->GetPreferredUniqueNetId();
        return true;
    }
    // 服务器逻辑
    else if (World->IsNetMode(NM_ListenServer) || World->IsNetMode(NM_DedicatedServer))
    {
        APlayerController* TargetPC = nullptr;

        if (World->IsNetMode(NM_ListenServer))
        {
            TargetPC = World->GetFirstPlayerController();
        }
        else if (World->IsNetMode(NM_DedicatedServer))
        {
            for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
            {
                TargetPC = It->Get();
                if (TargetPC && !TargetPC->IsLocalPlayerController())
                {
                    break;
                }
            }
        }

        if (!TargetPC || !TargetPC->PlayerState || !TargetPC->PlayerState->GetUniqueId().IsValid())
        {
            UE_LOG(LogOnlineSession, Error, TEXT("[服务端] 目标玩家ID无效，会话 %s"), *SessionName.ToString());
            return false;
        }
        OutNetId = TargetPC->PlayerState->GetUniqueId();
        return true;
    }

    UE_LOG(LogOnlineSession, Error, TEXT("无效网络模式，会话 %s"), *SessionName.ToString());
    return false;
}