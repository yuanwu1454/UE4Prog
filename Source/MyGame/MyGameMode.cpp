// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyGameMode.h"
#include "MyHUD.h"
#include "MyPlayerCharacter.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"
#include "OnlineSessionSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "ProjectNetworkUtils.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Log/MultiplayerLogHelper.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

AMyGameMode::AMyGameMode()
{
	// 核心：将HUDClass赋值为你的自定义HUD类
	HUDClass = AMyHUD::StaticClass();
    PlayerStateClass = AMyPlayerState::StaticClass();
}

// InitGame 的核心用途是 “游戏启动时的全局配置初始化”，常见场景包括：
// 解析启动参数，设置服务器 / 游戏核心规则；
// 根据地图加载差异化配置；
// 初始化全局游戏数据（如计分板、任务系统）；
// 校验启动参数合法性，终止非法启动。
void AMyGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    // 若父类初始化失败（ErrorMessage非空），直接返回
    if (!ErrorMessage.IsEmpty())
    {
        UE_LOG(LogGameMode, Error, TEXT("InitGame failed (super): %s"), *ErrorMessage);
        return;
    }
    
    // ========== 第二步：解析启动参数，初始化核心配置（核心逻辑） ==========
    // 1. 解析服务器最大玩家数（从启动Options中读取，默认32）
    // 2. 解析游戏模式类型（如Survival/Deathmatch）
    // 3. 根据地图名称初始化差异化配置
    MULTI_LOG(TEXT("InitGame"), this, this);
}

// InitGame：游戏刚 “开机”，只完成了 “系统设置”（如配置人数上限），但场景里的所有东西（角色、道具、AI）都还没 “加载出来”；
// BeginPlay：游戏 “正式开机完成”，场景里的所有东西都加载好了，能直接操作（比如生成初始 AI、开启倒计时）。
void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

    MULTI_LOG(TEXT("BeginPlay"), this, this);
}
// Login ↔ SpawnPlayerController ↔ PostLogin
// 场景2：服务器模式（客户端连接后自动初始化）
// 触发时机：玩家登录成功后立即调用（晚于 SpawnPlayerController，是登录流程的 “最后一步”）。
// 核心价值：这是第一个安全调用玩家控制器上 “复制函数（Replicated Functions）” 的时机。
// 原因：UE 的网络复制系统在登录完成前未完全初始化，过早调用 Replicated 函数会导致同步失效、崩溃或数据丢失；
// Replicated 函数：标记为UFUNCTION(Server/Client/NetMulticast)的网络函数，用于客户端 - 服务端数据同步。
// PostLogin 是服务端独占的回调（客户端永远不会执行这个函数），所以如果要让 “登录成功” 这个事件体现在客户端（比如显示 UI、播放音效），
// 就必须通过 UE 的网络 RPC（远程过程调用） 机制，由服务端主动 “调用 / 广播” 客户端的函数。
void AMyGameMode::PostLogin(APlayerController* NewPlayer)
{
    // MULTI_LOG(FString::Printf(TEXT("PostLogin Begin：%s"), *NewPlayer->GetName()), this, NewPlayer); 
    Super::PostLogin(NewPlayer);
    // 复杂逻辑移到 PostLogin。
    MULTI_LOG(FString::Printf(TEXT("PostLogin End：%s"), *NewPlayer->GetName()), this, NewPlayer); 
}

TSubclassOf<AGameSession> AMyGameMode::GetGameSessionClass() const
{
    return AMyGameSession::StaticClass();
}

void AMyGameMode::Logout(AController* Exiting)
{
    // MULTI_LOG(FString::Printf(TEXT("PostLogin Begin：%s"), *Exiting->GetName()), this, Exiting); 
    Super::Logout(Exiting);
    MULTI_LOG(FString::Printf(TEXT("PostLogin End：%s"), *Exiting->GetName()), this, Exiting); 
}

// 专门用于初始化 GameState 的核心函数
// 首次初始化：GameState 被生成后，在 GameMode 的 PreInitializeComponents() 阶段调用（介于 InitGame 和 BeginPlay 之间）；
// 重置场景：调用 GameMode 的 Reset() 函数时（如游戏重新开始、场景重置），会再次调用以重新初始化 GameState。
// GameMode 将自己的核心配置 “注入” 到 GameState 中，让 GameState 成为全局状态的统一出口。
void AMyGameMode::InitGameState()
{
    Super::InitGameState();
    if(GameState)
    {
        // ========== 第三步：将GameMode配置同步到GameState（核心逻辑） ==========
        // 1. 同步基础配置
        // 2. 初始化全局计时/计分
        // 3. 初始化自定义游戏规则
    }
}

// 核心设计目的：让开发者自定义玩家控制器的创建逻辑
// InRemoteRole	ENetRole	该控制器的网络角色（UE 网络同步核心枚举），决定控制器在客户端 / 服务端的角色（如权威端 / 模拟端）。
// Options	const FString&	玩家登录 URL 中的自定义参数字符串（如?PlayerID=123&Mode=Survival），用于传递登录配置。
APlayerController* AMyGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
    FString DisplayStr = UEnum::GetDisplayValueAsText(InRemoteRole).ToString();
    MULTI_LOG(FString::Printf(TEXT("SpawnPlayerController RemoteRole: %s Option: %s "), *DisplayStr, *Options), this, this);
    
    // 1. 解析Options参数（提取是否为VIP）
    // FString IsVIP;
    // if (FParse::Value(*Options, TEXT("VIP="), IsVIP))
    // {
    //     // 2. 如果是VIP，创建VIP专属控制器
    //     if (IsVIP.Equals(TEXT("1"), ESearchCase::IgnoreCase))
    //     {
    //         return GetWorld()->SpawnActor<AVipPlayerController>(AVipPlayerController::StaticClass(), FTransform::Identity);
    //     }
    // }
    return Super::SpawnPlayerController(InRemoteRole, Options);
}

// Login 函数的核心重写场景是：自定义登录校验（如服务器人数限制、账号封禁校验、参数合法性检查）
APlayerController* AMyGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
    const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    // ========== 第一步：自定义登录校验（核心重写逻辑） ==========
    // 1. 检查服务器是否满员
    // 2. 检查玩家是否被封禁（示例：模拟封禁列表）
    // 3. 校验自定义参数合法性（如必须传入PlayerName）
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode Login")), this, this);
    // ========== 第二步：调用父类默认登录逻辑（必须！） ==========
    auto NewPC = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

    // 父类登录失败则直接返回
    if (!NewPC || !ErrorMessage.IsEmpty())
    {
        return NewPC;
    }
    // ========== 第三步：基础玩家属性设置（仅基础逻辑！） ==========
    // 示例：设置玩家名称（基础逻辑，非游戏业务）
    return NewPC;
}

// 本质上输出参数就是ErrorMessage，这才是唯一要修改的地方。 前置校验
// ErrorMessage 不为空就会断开
void AMyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
    FString& ErrorMessage)
{
    // ========== 第一步：调用父类默认实现（建议保留，兼容UE原生逻辑） ==========
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
    
    // 若父类已拒绝登录（ErrorMessage非空），直接返回
    if (!ErrorMessage.IsEmpty())
    {
        return;
    }

    // ========== 第二步：自定义前置校验逻辑（核心） ==========
    // 1. 校验服务器状态：维护中则拒绝登录
    // 2. IP封禁校验（示例：封禁特定IP段）
    // 3. 重复登录校验（示例：检查UniqueId是否已在线）
    // 4. 自定义参数校验（必须携带AntiCheatToken）

    // ========== 第三步：校验通过，无需处理（ErrorMessage保持为空） ==========
    UE_LOG(LogGameMode, Log, TEXT("PreLogin passed: %s (IP: %s)"), *UniqueId.ToString(), *Address);
}

// GameWelcomePlayer 的核心价值是 “登录阶段的服务器→客户端定制化通信”
void AMyGameMode::GameWelcomePlayer(UNetConnection* Connection, FString& RedirectURL)
{
    // 1. 调用父类默认实现（保留UE原生逻辑）
    Super::GameWelcomePlayer(Connection, RedirectURL);
    if (!Connection) { return; }
    // 方式1：简单重定向（UE自动发送重定向指令）
    // 方式2：发送自定义游戏消息（更灵活，客户端可弹窗提示）
}
