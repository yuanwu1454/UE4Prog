// Fill out your copyright notice in the Description page of Project Settings.

// MyGameSession.cpp
#include "MyGameSession.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

// Sets default values
AMyGameSession::AMyGameSession()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyGameSession::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyGameSession::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyGameSession::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    // 玩家登录后：注册到在线会话
    if (NewPlayer->PlayerState && NewPlayer->PlayerState->GetUniqueId().IsValid())
    {
        IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
        if (OnlineSub)
        {
            SessionInterface = OnlineSub->GetSessionInterface();
            if (SessionInterface.IsValid())
            {
                SessionInterface->RegisterPlayer(SessionName, *NewPlayer->PlayerState->GetUniqueId().GetUniqueNetId(), false);
                UE_LOG(LogGameSession, Log, TEXT("玩家 %s 注册到会话 %s"), *NewPlayer->GetName(), *SessionName.ToString());
            }
        }
    }
}

void AMyGameSession::NotifyLogout(const APlayerController* PC)
{
    Super::NotifyLogout(PC);
    // 玩家登出：取消注册
    if (PC->PlayerState && PC->PlayerState->GetUniqueId().IsValid())
    {
        IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
        if (OnlineSub)
        {
            SessionInterface = OnlineSub->GetSessionInterface();
            if (SessionInterface.IsValid())
            {
                SessionInterface->UnregisterPlayer(SessionName, *PC->PlayerState->GetUniqueId().GetUniqueNetId());
            }
        }
    }
}
// 辅助方法：安全获取当前GameMode
AGameModeBase* AMyGameSession::GetCurrentGameMode() const
{
    if (!GetWorld())
    {
        UE_LOG(LogGameSession, Error, TEXT("GameSession无有效World，无法获取GameMode"));
        return nullptr;
    }
    return GetWorld()->GetAuthGameMode(); // 仅服务器有效（GameSession只在服务器运行）
}

// 辅助方法：获取当前在线玩家数
int32 AMyGameSession::GetCurrentPlayerCount() const
{
    AGameModeBase* GameMode = GetCurrentGameMode();
    return GameMode ? GameMode->GetNumPlayers() : 0;
}

// 辅助方法：获取当前观众数
int32 AMyGameSession::GetCurrentSpectatorCount() const
{
    AGameModeBase* GameMode = GetCurrentGameMode();
    if (!GameMode) return 0;

    // 方式1：如果是自定义GameMode，可直接添加GetNumSpectators()方法
    // 方式2：通用方式（遍历所有PlayerController，判断是否为观众）
    int32 SpectatorCount = 0;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC && PC->GetPlayerState<APlayerState>()&& PC->GetPlayerState<APlayerState>()->IsSpectator())
        {
            SpectatorCount++;
        }
    }
    return SpectatorCount;
}

// 重写AtCapacity：自定义满员判断
bool AMyGameSession::AtCapacity(bool bSpectator)
{
    AGameModeBase* GameMode = GetCurrentGameMode();
    if (!GameMode)
    {
        return true; // 无GameMode时默认满员，拒绝加入
    }

    // 观众满员判断
    if (bSpectator)
    {
        // 使用GameMode的MaxSpectators（需在GameMode中配置）
        return GetCurrentSpectatorCount() >=MaxSpectators;
    }
    // 玩家满员判断（预留1个管理员位置）
    return GetCurrentPlayerCount() >= (MaxPlayers - 1);
}

void AMyGameSession::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogGameSession, Log, TEXT("会话 %s 创建成功！"), *InSessionName.ToString());
        RegisterServer();
    }
    else
    {
        // 获取在线子系统，绑定会话回调
        IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
        if (OnlineSub)
        {
            if (SessionInterface.IsValid())
            {
                FString ErrorReason = TEXT("未知原因，UE4.27常见排查点：");
        
                if (SessionInterface.IsValid())
                {
                    // 原因1：同名会话已存在（UE4.27最常见）
                    if (SessionInterface->GetNamedSession(InSessionName) != nullptr)
                    {
                        ErrorReason = FString::Printf(TEXT("会话 %s 已存在（同名冲突）"), *InSessionName.ToString());
                    }
                    // 原因2：Null子系统未启用（通过子系统状态判断，替代FOnlineSubsystemNullSettings）
                    else if (IOnlineSubsystem::Get()->GetSubsystemName() == "Null")
                    {
                        IOnlineSubsystem* NullSubsystem = IOnlineSubsystem::Get("Null");
                        if (!NullSubsystem || !NullSubsystem->IsEnabled())
                        {
                            ErrorReason = TEXT("Null在线子系统未启用（检查DefaultEngine.ini的bEnabled=true）");
                        }
                        else
                        {
                            ErrorReason = TEXT("Null子系统已启用，但LAN模式可能未配置（检查bUseLANBroadcasts=true）");
                        }
                    }
                    // 原因3：Steam子系统初始化失败（如果使用Steam）
                    else if (IOnlineSubsystem::Get()->GetSubsystemName() == "Steam")
                    {
                        ErrorReason = TEXT("Steam子系统初始化失败（检查Steam AppId/客户端运行状态）");
                    }
                }
                else
                {
                    ErrorReason = TEXT("SessionInterface为空（在线子系统未初始化）");
                }

                // 打印最终错误原因
                UE_LOG(LogGameSession, Error, TEXT("会话 %s 创建失败！原因：%s"), 
                    *InSessionName.ToString(), *ErrorReason);
            }
        }
    }
}

bool AMyGameSession::CreateSession(FName InSessionName, int32 InMaxPlayers)
{
    // 1. 仅服务器可创建
    if (!IsServer())
    {
        UE_LOG(LogGameSession, Warning, TEXT("拒绝客户端创建会话！仅服务器可执行"));
        return false;
    }

    if (!InitSessionInterface())
    {
        UE_LOG(LogGameSession, Error, TEXT("在线子系统未初始化，无法创建会话 %s"), *InSessionName.ToString());
        return false;
    }

    // 3. 校验是否已有同名 Session
    if (SessionInterface->GetNamedSession(InSessionName) != nullptr)
    {
        UE_LOG(LogGameSession, Warning, TEXT("会话 %s 已存在，无需重复创建"), *InSessionName.ToString());
        return false;
    }


    // 4. 清理旧回调 + 绑定创建完成回调
    SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
    SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
        FOnCreateSessionCompleteDelegate::CreateUObject(this, &AMyGameSession::OnCreateSessionComplete)
    );

    SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
    SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &AMyGameSession::OnJoinSessionComplete)
    );

    // 5. 构建 Session 设置（UE4.27 LAN 模式核心配置）
    FOnlineSessionSettings SessionSettings;
    SessionSettings.bIsLANMatch = true;          // LAN 模式开关（公网需改 false + 配置 Steam/EOS）
    SessionSettings.bUsesPresence = false;       // 非 Presence 会话
    SessionSettings.NumPublicConnections = InMaxPlayers; // 最大玩家数
    SessionSettings.bAllowJoinInProgress = true; // 允许中途加入
    SessionSettings.bAllowJoinViaPresence = true;
    SessionSettings.bShouldAdvertise = true;     // 对外广播会话

    // 6. 获取本地 PlayerID（服务器端必须）
    FUniqueNetIdRepl NetId;
    if (!GetValidPlayerNetId( NetId, InSessionName))
    {
        UE_LOG(LogGameSession, Error, TEXT("本地 PlayerID 无效，无法创建会话 %s"), *InSessionName.ToString());
        SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
        return false;
    }

    // 7. 异步创建 Session
    bool bCreateSuccess = SessionInterface->CreateSession(
        *NetId,
        InSessionName,
        SessionSettings
    );

    if (!bCreateSuccess)
    {
        UE_LOG(LogGameSession, Error, TEXT("同步调用 CreateSession 失败！会话 %s"), *InSessionName.ToString());
        SessionInterface->ClearOnCreateSessionCompleteDelegates(this);
        return false;
    }

    SessionName = InSessionName;

    UE_LOG(LogGameSession, Log, TEXT("已发起创建会话 %s 的异步请求（最大玩家数：%d）"), *InSessionName.ToString(), MaxPlayers);
    return true;
}

bool AMyGameSession::DestroySession(FName InSessionName)
{
    // 1. 仅服务器可销毁
    if (!IsServer())
    {
        UE_LOG(LogGameSession, Warning, TEXT("拒绝客户端销毁会话！仅服务器可执行"));
        return false;
    }

    if (InitSessionInterface())
    {
        UE_LOG(LogGameSession, Error, TEXT("SessionInterface初始化失败，无法销毁会话 %s"), *InSessionName.ToString());
        return false;
    }

    // 3. 校验Session是否存在
    if (SessionInterface->GetNamedSession(InSessionName) == nullptr)
    {
        UE_LOG(LogGameSession, Warning, TEXT("会话 %s 不存在，无需销毁"), *InSessionName.ToString());
        return false;
    }

    // 4. 绑定销毁回调
    SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
        FOnDestroySessionCompleteDelegate::CreateUObject(this, &AMyGameSession::OnDestroySessionComplete)
    );

    // 5. 调用异步销毁
    bool bDestroySuccess = SessionInterface->DestroySession(InSessionName);
    if (!bDestroySuccess)
    {
        UE_LOG(LogGameSession, Error, TEXT("同步调用DestroySession失败！会话 %s"), *InSessionName.ToString());
        SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
        return false;
    }

    UE_LOG(LogGameSession, Log, TEXT("已发起销毁会话 %s 的异步请求"), *InSessionName.ToString());
    return true;
}

void AMyGameSession::OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogGameSession, Log, TEXT("会话 %s 销毁成功！"), *InSessionName.ToString());
        SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
        ResetSessionState();
    }
    else
    {
        UE_LOG(LogGameSession, Error, TEXT("会话 %s 销毁失败！"), *InSessionName.ToString());
        SessionInterface->ClearOnDestroySessionCompleteDelegates(this);
    }
}

bool AMyGameSession::IsServer() const
{
    ENetMode NetMode = GetWorld()->GetNetMode();
    return NetMode == NM_DedicatedServer || NetMode == NM_ListenServer;
}

void AMyGameSession::ResetSessionState()
{
    UE_LOG(LogGameSession, Log, TEXT("Session状态已重置"));
}

bool AMyGameSession::InitSessionInterface()
{
    if (!SessionInterface.IsValid())
    {
        IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
        if (OnlineSubsystem)
        {
            SessionInterface = OnlineSubsystem->GetSessionInterface();
        }
        else
        {
            UE_LOG(LogGameSession, Error, TEXT("OnlineSubsystem is not found"));
            return false;
        }
    }
    return true;
}


// 第四步：加入Session完成的回调（客户端）
void AMyGameSession::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type Result)
{
    // 解析加入结果（UE4.27枚举，替代GetLastError）
    FString ResultStr;
    switch (Result)
    {
    case EOnJoinSessionCompleteResult::Success:
        ResultStr = TEXT("成功");
        // 加入成功：获取连接字符串并跳转至服务器地图
        if (SessionInterface.IsValid())
        {
            FString ConnectString;
            if (SessionInterface->GetResolvedConnectString(InSessionName, ConnectString))
            {
                APlayerController* PC = GetWorld()->GetFirstPlayerController();
                if (PC)
                {
                    // 客户端跳转到服务器地图（核心步骤）
                    PC->ClientTravel(ConnectString, TRAVEL_Absolute);
                    UE_LOG(LogGameSession, Log, TEXT("客户端跳转到服务器：%s"), *ConnectString);
                }
            }
        }
        break;
    case EOnJoinSessionCompleteResult::SessionDoesNotExist:
        ResultStr = TEXT("会话不存在");
        break;
    case EOnJoinSessionCompleteResult::SessionIsFull:
        ResultStr = TEXT("会话已满");
        break;
    case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
        ResultStr = TEXT("无法获取服务器地址");
        break;
    default:
        ResultStr = TEXT("未知错误");
        break;
    }

    UE_LOG(LogGameSession, Log, TEXT("客户端加入会话 %s 结果：%s"), *InSessionName.ToString(), *ResultStr);
    SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
}
// 第一步：客户端查找LAN内的可用Session
bool AMyGameSession::FindAvailableSessions()
{
    // 1. 仅纯客户端可执行（服务器/主机无需查找）
    if (!IsPureClient())
    {
        UE_LOG(LogGameSession, Warning, TEXT("仅纯客户端可查找会话！"));
        return false;
    }

    // 2. 初始化SessionInterface（客户端懒加载）
    if (!SessionInterface.IsValid())
    {
        IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
        if (OnlineSubsystem)
        {
            SessionInterface = OnlineSubsystem->GetSessionInterface();
            UE_LOG(LogGameSession, Log, TEXT("客户端懒加载初始化SessionInterface成功"));
        }
        else
        {
            UE_LOG(LogGameSession, Error, TEXT("客户端在线子系统未初始化，无法查找会话"));
            return false;
        }
    }

    // 3. 清理旧回调 + 绑定查找完成回调
    SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
    SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
        FOnFindSessionsCompleteDelegate::CreateUObject(this, &AMyGameSession::OnFindSessionsComplete)
    );

    // 4. 构建查找配置（LAN模式）
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (!SessionSearch.IsValid())
    {
        UE_LOG(LogGameSession, Error, TEXT("客户端创建SessionSearch失败"));
        return false;
    }
    SessionSearch->bIsLanQuery = true;       // LAN查找（公网改false）
    SessionSearch->MaxSearchResults = 10;     // 最大查找结果数
    SessionSearch->PingBucketSize = 50;       // 延迟桶大小（可选）

    // 5. 异步查找Session
    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
    {
        UE_LOG(LogGameSession, Error, TEXT("客户端PlayerID无效，无法查找会话"));
        return false;
    }

    bool bFindSuccess = SessionInterface->FindSessions(
        *LocalPlayer->GetPreferredUniqueNetId(),
        SessionSearch.ToSharedRef()
    );

    if (!bFindSuccess)
    {
        UE_LOG(LogGameSession, Error, TEXT("客户端同步调用FindSessions失败"));
        SessionInterface->ClearOnFindSessionsCompleteDelegates(this);
        SessionSearch.Reset();
        return false;
    }

    UE_LOG(LogGameSession, Log, TEXT("客户端已发起查找LAN会话的异步请求"));
    return true;
}

// 第二步：查找Session完成的回调（客户端）
void AMyGameSession::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (bWasSuccessful && SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
    {
        UE_LOG(LogGameSession, Log, TEXT("客户端找到 %d 个可用会话"), SessionSearch->SearchResults.Num());
        
        // 示例：加入第一个找到的Session（也可根据SessionName筛选）
        FOnlineSessionSearchResult FirstResult = SessionSearch->SearchResults[0];
        JoinGameSession(FName("GameSession_01"), FirstResult);
    }
    else
    {
        UE_LOG(LogGameSession, Error, TEXT("客户端未找到可用会话！"));
        SessionSearch.Reset();
    }
}

// 第三步：客户端加入指定Session
bool AMyGameSession::JoinGameSession(FName InSessionName, const FOnlineSessionSearchResult& SessionResult)
{
    if (!IsPureClient())
    {
        UE_LOG(LogGameSession, Warning, TEXT("仅纯客户端可加入会话！"));
        return false;
    }

    if (!SessionInterface.IsValid() || !SessionResult.IsValid())
    {
        UE_LOG(LogGameSession, Error, TEXT("客户端SessionInterface/会话结果无效，无法加入"));
        return false;
    }

    // 清理旧回调 + 绑定加入完成回调
    SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
    SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
        FOnJoinSessionCompleteDelegate::CreateUObject(this, &AMyGameSession::OnJoinSessionComplete)
    );

    // 异步加入Session
    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    bool bJoinSuccess = SessionInterface->JoinSession(
        *LocalPlayer->GetPreferredUniqueNetId(),
        SessionName,
        SessionResult
    );

    if (!bJoinSuccess)
    {
        UE_LOG(LogGameSession, Error, TEXT("客户端同步调用JoinSession失败"));
        SessionInterface->ClearOnJoinSessionCompleteDelegates(this);
        return false;
    }

    UE_LOG(LogGameSession, Log, TEXT("客户端已发起加入会话 %s 的异步请求"), *InSessionName.ToString());
    return true;
}


// 补充：纯客户端判断函数（MyGameSession.h中声明）
bool AMyGameSession::IsPureClient() const
{
    return GetWorld()->GetNetMode() == NM_Client;
}

// 6. 获取 PlayerID（区分客户端/服务端）
bool AMyGameSession::GetValidPlayerNetId(FUniqueNetIdRepl& OutNetId, const FName& InSessionName)
{
    // 客户端：使用 LocalPlayer 获取（原有逻辑）
    if (GetWorld()->IsNetMode(NM_Client))
    {
        ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
        if (!LocalPlayer || !LocalPlayer->GetPreferredUniqueNetId().IsValid())
        {
            UE_LOG(LogGameSession, Error, TEXT("[客户端] 本地 PlayerID 无效，无法创建会话 %s"), *InSessionName.ToString());
            return false;
        }
        OutNetId = LocalPlayer->GetPreferredUniqueNetId();
        return true;
    }
    // 服务端：监听服务器/专用服务器逻辑
    else if (GetWorld()->IsNetMode(NM_ListenServer) || GetWorld()->IsNetMode(NM_DedicatedServer))
    {
        APlayerController* TargetPC = nullptr;

        // ===== 场景1：监听服务器 - 获取主机玩家的PlayerController =====
        if (GetWorld()->IsNetMode(NM_ListenServer))
        {
            // 监听服务器的本地控制器 = 主机玩家的PlayerController
            TargetPC = GetWorld()->GetFirstPlayerController();
        }
        // ===== 场景2：专用服务器 - 从已连接玩家列表中获取（比如第一个玩家） =====
        else if (GetWorld()->IsNetMode(NM_DedicatedServer))
        {
            // 方式A：获取已连接的第一个玩家控制器（适合“创建公共会话”场景）
            for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
            {
                TargetPC = It->Get();
                if (TargetPC && TargetPC->IsLocalPlayerController() == false) // 排除本地（专用服务器无本地）
                    {
                    break;
                    }
            }

            // 方式B：如果你有“目标玩家ID”，可通过ID匹配PlayerController（更精准）
            // FString TargetPlayerId = "要匹配的玩家ID"; // 比如SteamID
            // for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
            // {
            //     APlayerController* PC = It->Get();
            //     if (PC && PC->PlayerState && PC->PlayerState->GetUniqueId().ToString() == TargetPlayerId)
            //     {
            //         TargetPC = PC;
            //         break;
            //     }
            // }
        }

        // 验证PlayerController和NetId有效性
        if (!TargetPC || !TargetPC->PlayerState || !TargetPC->PlayerState->GetUniqueId().IsValid())
        {
            UE_LOG(LogGameSession, Error, TEXT("[服务端] 目标玩家的 PlayerID 无效，无法创建会话 %s"), *InSessionName.ToString());
            return false;
        }
        OutNetId = TargetPC->PlayerState->GetUniqueId();
        return true;
    }

    UE_LOG(LogGameSession, Error, TEXT("无效的网络模式，无法获取 PlayerID %s"), *InSessionName.ToString());
    return false;
}