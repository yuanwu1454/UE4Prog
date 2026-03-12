// OnlineSessionSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSubsystem.generated.h"

// 会话操作结果委托（供外部监听）
DECLARE_DELEGATE_TwoParams(FOnSessionOperationComplete, FName /* SessionName */, bool /* bSuccess */);
DECLARE_DELEGATE_OneParam(FOnFindSessionsResultComplete, const TArray<FOnlineSessionSearchResult>& /* FoundSessions */);

UCLASS()
class MYGAME_API UOnlineSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    // ===== 核心接口（外部调用）=====
    // 服务器创建会话
    UFUNCTION(BlueprintCallable, Category = "Online|Session")
    bool CreateSession(FName SessionName, int32 MaxPlayers, bool bIsLAN = true);

    // 服务器销毁会话
    UFUNCTION(BlueprintCallable, Category = "Online|Session")
    bool DestroySession(FName SessionName);

    // 客户端查找会话
    UFUNCTION(BlueprintCallable, Category = "Online|Session")
    bool FindAvailableSessions(bool bIsLAN = true, int32 MaxResults = 10);

    // 客户端加入会话
    // UFUNCTION(BlueprintCallable, Category = "Online|Session")
    bool JoinGameSession(FName SessionName, const FOnlineSessionSearchResult& SessionResult);

    // 注册玩家到会话（服务器）
    // UFUNCTION(BlueprintCallable, Category = "Online|Session")
    bool RegisterPlayerToSession(FName SessionName, const FUniqueNetIdRef& PlayerNetId);

    // 取消玩家注册（服务器）
    // UFUNCTION(BlueprintCallable, Category = "Online|Session")
    bool UnregisterPlayerFromSession(FName SessionName, const FUniqueNetIdRef& PlayerNetId);

    // ===== 委托注册（供外部绑定）=====
    void BindCreateSessionComplete(const FOnSessionOperationComplete& Delegate) { OnCreateSessionCompleteDelegate = Delegate; }
    void BindDestroySessionComplete(const FOnSessionOperationComplete& Delegate) { OnDestroySessionCompleteDelegate = Delegate; }
    void BindFindSessionsComplete(const FOnFindSessionsResultComplete& Delegate) { OnFindSessionsCompleteDelegate = Delegate; }
    void BindJoinSessionComplete(const FOnJoinSessionComplete& Delegate) { OnJoinSessionCompleteDelegate = Delegate; }
    
    // 安全获取 SessionInterface
    bool InitSessionInterface();

    // 会话搜索对象（客户端）
    TSharedPtr<FOnlineSessionSearch> SessionSearch;
    
private:
    // OnlineSubsystem 核心接口
    IOnlineSubsystem* OnlineSubsystem = nullptr;
    IOnlineSessionPtr SessionInterface;



    // 委托对象
    FOnSessionOperationComplete OnCreateSessionCompleteDelegate;
    FOnSessionOperationComplete OnDestroySessionCompleteDelegate;
    FOnFindSessionsResultComplete OnFindSessionsCompleteDelegate;
    FOnJoinSessionComplete OnJoinSessionCompleteDelegate;

    // ===== 内部回调 =====
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    // 辅助：获取有效NetId（区分客户端/服务器）
    bool GetValidPlayerNetId(FUniqueNetIdRepl& OutNetId, FName SessionName);
};