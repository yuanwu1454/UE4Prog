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
    // 方式1：直接设置固定文本（非本地化）
    DefaultPlayerName = FText::FromString(TEXT("游客"));
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
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode InitGameState")), this, this);
    Super::InitGameState();
    if(GameState)
    {
        // ========== 第三步：将GameMode配置同步到GameState（核心逻辑） ==========
        // 1. 同步基础配置
        // 2. 初始化全局计时/计分
        // 3. 初始化自定义游戏规则
    }
}


// 无缝旅行（Seamless）	不销毁 GameMode/GameState，玩家控制器保持连接，地图切换无卡顿	开放世界关卡衔接、副本进入
// 硬旅行（Hard）	销毁当前世界所有 Actor，重启 GameMode，玩家需重新登录	主菜单→游戏关卡、跨大地图切换
// StartToLeaveMap 是地图切换前最后一个可安全操作当前地图 Actor 的时机，一旦该函数执行完毕，UE 会开始卸载当前地图的资源，此时再操作 Actor 可能导致崩溃。
void AMyGameMode::StartToLeaveMap()
{
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode StartToLeaveMap")), this, this);
    Super::StartToLeaveMap();
    // ========== 第二步：核心收尾逻辑（按优先级执行） ==========
    // 1. 保存所有玩家进度（最高优先级，避免数据丢失）
    // 2. 清理当前地图的临时资源（AI、动态道具、特效）
    // 3. 停止当前地图的游戏逻辑（如倒计时、音效、网络同步）
    // 4. 通知所有客户端显示加载动画（通过NetMulticast RPC）
    // 5. 关闭当前地图的网络同步（可选，硬旅行时推荐）
}

// 无缝地图旅行（Seamless Travel）的核心收尾回调
// 专门作用于新地图的 GameMode，是无缝切换后恢复玩家状态、重新初始化游戏逻辑的关键入口
// 无缝旅行的 “最后一步”，弥补无缝切换后玩家不会触发Login()的缺陷，确保新地图的 GameMode 能正确初始化已有玩家
// 为啥不会触发 Login，因为无缝旅行时，玩家控制器（PlayerController）会被保留，因此不会重新执行Login()/PostLogin()流程（这两个函数仅在玩家首次登录时触发）；
// 不销毁核心对象：GameMode（新地图的会替换旧的）、PlayerController、PlayerState 会被保留，玩家无需重新登录；
void AMyGameMode::PostSeamlessTravel()
{
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode PostSeamlessTravel")), this, this);
    Super::PostSeamlessTravel();

    // ========== 第二步：核心初始化逻辑（按优先级执行） ==========
    // 1. 初始化新地图的GameState（同步游戏规则）
    // 2. 遍历所有保留的玩家控制器，初始化每个玩家 的ui, pawn
}

// 这段代码是 Unreal Engine（UE）GameMode 类中无缝旅行（Seamless Travel）的玩家级初始化函数，
// 是PostSeamlessTravel的 “精细化补充”，专门针对单个玩家控制器处理无缝切换后的重初始化逻辑。以下是完整解析：
// PostSeamlessTravel 是 “广播通知所有玩家集合”，而HandleSeamlessTravelPlayer 是 “单独叫出某个迟到的玩家，单独交代事情”。
// PostSeamlessTravel() 会调用 HandleSeamlessTravelPlayer
void AMyGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
    Super::HandleSeamlessTravelPlayer(C);
    // ========== 第二步：类型校验（确保是玩家控制器） ==========
    // ========== 第三步：单个玩家的精细化初始化（核心逻辑） ==========
    // 1. 修复玩家Possess状态（避免无缝旅行后无角色）
    // 2. 恢复该玩家的专属数据（如职业、任务进度，区别于批量恢复的通用数据）
    // 3. 为滞后加载的玩家补发新地图公告（避免遗漏）
    // 4. 校验并修复玩家输入/网络状态（针对延迟玩家的特殊处理）
}

// 无缝旅行（Seamless Travel）专属的玩家控制器类选择函数，核心作用是决定 “无缝切换地图时，新地图应使用哪个 PlayerController 子类来生成 / 替换玩家控制器”
// 在保留玩家连接的前提下，动态切换 PlayerController 的类，适配新地图的逻辑需求。
// 按地图类型切换控制器（主城→副本、PVE→PVP）；
// 按玩家身份切换控制器（VIP 玩家→专属控制器、管理员→GM 控制器）；
// 按游戏模式切换控制器（生存模式→战斗控制器、休闲模式→交互控制器）。
TSubclassOf<APlayerController> AMyGameMode::GetPlayerControllerClassToSpawnForSeamlessTravel(
    APlayerController* PreviousPlayerController)
{
    return Super::GetPlayerControllerClassToSpawnForSeamlessTravel(PreviousPlayerController);
}

// 无缝旅行时替换玩家控制器的核心函数，专门处理 “新旧 PlayerController 切换” 的全流程（包括视口绑定、网络连接、玩家状态迁移）
// 仅仅将旧的controller的数据等同步到新的controller
void AMyGameMode::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC)
{
    Super::SwapPlayerControllers(OldPC, NewPC);
    // ========== 第二步：校验参数有效性 ==========
     // ========== 第三步：自定义迁移逻辑（核心扩展） ==========
    // 1. 迁移旧控制器的自定义数据（如临时UI状态、输入配置、战斗缓存）
    // 2. 修复新控制器的输入/视口状态（避免切换后按键无响应）
    // 3. 同步新控制器的网络权限（如VIP玩家的高优先级网络）
    // 4. 清理旧控制器的残留资源（避免内存泄漏）
}

// “无缝切换地图时，哪些动态生成的 Actor 需要从旧地图迁移到新地图”，是实现 “跨地图保留关键 Actor” 的核心入口
// 自动迁移的 Actor（无需手动添加）：
// 所有PlayerController实例；
// 角色权限 < ROLE_Authority 的 Actor（客户端本地 Actor）；例如：客户端本地特效 Actor
// 属于 “保留 Actor 的子对象” 的非 Actor 对象（如 Actor 的组件、变量引用的对象）；
// 仅能迁移的 Actor 范围：
// 仅PersistentLevel中的动态 Actor（游戏运行时生成的 Actor，如 Spawn 的 AI、道具）；
// 静态放置的地图 Actor（如场景模型）无法迁移，会随旧地图销毁；
void AMyGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
// bToTransition：布尔值，标记旅行阶段：
// true：从旧地图→过渡地图（无缝旅行的第一阶段）；
// false：从过渡地图→新地图（无缝旅行的第二阶段）；
    Super::GetSeamlessTravelActorList(bToTransition, ActorList);

    // ========== 第二步：按旅行阶段添加自定义Actor ==========
    // 1. 所有阶段都保留：全局游戏管理器（核心系统，必须跨地图存活）
    // 2. 所有阶段都保留：玩家的宠物Actor（跟随玩家跨地图）
    // 3. 所有阶段都保留：玩家携带的关键任务道具
    // 4. 差异化处理：仅过渡阶段保留加载UI Actor（新地图加载完成后销毁）
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
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode PreLogin")), this, this);
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
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode GameWelcomePlayer")), this, this);
    // 1. 调用父类默认实现（保留UE原生逻辑）
    Super::GameWelcomePlayer(Connection, RedirectURL);
    if (!Connection) { return; }
    // 方式1：简单重定向（UE自动发送重定向指令）
    // 方式2：发送自定义游戏消息（更灵活，客户端可弹窗提示）
}

bool AMyGameMode::MustSpectate_Implementation(APlayerController* NewPlayerController) const
{
    return Super::MustSpectate_Implementation(NewPlayerController);
}

// 函数类型：BlueprintNativeEvent（蓝图原生事件）→ 支持 C++ 重写，也支持在蓝图中覆写（HandleStartingNewPlayer_Implementation）；
// 核心定位：玩家 “从登录到进入游戏” 的最后一步回调，是初始化玩家游戏状态、触发游戏开局逻辑的关键入口。
void AMyGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode HandleStartingNewPlayer_Implementation")), this, this);
    // ========== 第一步：调用父类默认实现（核心！保留基础逻辑） ==========
    // 父类会完成：生成默认Pawn、绑定Pawn到Controller、初始化基础输入
    Super::HandleStartingNewPlayer_Implementation(NewPlayer);

    // ========== 第二步：自定义玩家初始化逻辑（核心扩展） ==========
    // 1. 替换父类生成的默认Pawn，生成自定义玩家角色
    // 2. 初始化玩家游戏状态（加载存档、同步分数/道具）
    // 3. 给新玩家发送初始化数据（新手引导、服务器公告）
    
}
// DefaultPlayerName 是玩家的 “初始默认名”，玩家后续可通过ChangeName函数修改，两者互不冲突 ——DefaultPlayerName仅作用于 “首次无名称加入” 的场景。

void AMyGameMode::ChangeName(AController* Controller, const FString& NewName, bool bNameChange)
{
    // ========== 第二步：自定义规则校验（核心扩展） ==========
    // 1. 名称合法性校验
    // 2. 名称唯一性校验
    // 3. 改名权限校验（仅修改时检查，首次设置不限制）
    Super::ChangeName(Controller, NewName, bNameChange);
    // ========== 第四步：自定义收尾逻辑 ==========
    // 1. 记录改名日志（成功）
    // 2. 若为修改名称，触发冷却/扣减道具
    // 3. 通知客户端：改名成功
}
// FindPlayerStart 调用ChoosePlayerStart

AActor* AMyGameMode::ChoosePlayerStart_Implementation(AController* Player)
{

    // 2. 第二步筛选：排除被占用的出生点（精准碰撞检测）
    // 返回的一般都是APlayerStart
    return Super::ChoosePlayerStart_Implementation(Player);
}

// FinishRestartPlayer 是 UE 玩家重启流程的收尾虚函数，负责 Pawn 绑定、旋转设置、输入启用等核心收尾操作；
void AMyGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode FinishRestartPlayer")), this, this);
    Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

// 处理所有玩家初始化逻辑中 “跨场景切换方式共享”
// 无论玩家是首次登录（PostLogin）还是无缝场景切换（HandleSeamlessTravelPlayer），都会调用该函数完成基础初始化
void AMyGameMode::GenericPlayerInitialization(AController* C)
{
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode GenericPlayerInitialization")), this, this);
    Super::GenericPlayerInitialization(C);

    // 2. 自定义通用初始化逻辑（所有玩家初始化场景都执行）
    // 自定义逻辑1：加载玩家存档数据（首次登录/无缝切换都要加载）
    // 自定义逻辑2：初始化成就系统
    // 自定义逻辑3：绑定玩家数据到UI（同步到客户端）
}

void AMyGameMode::InitializeHUDForPlayer_Implementation(APlayerController* NewPlayer)
{
    MULTI_LOG(FString::Printf(TEXT("AMyGameMode GenericPlayerInitialization")), this, this);
    Super::InitializeHUDForPlayer_Implementation(NewPlayer);
}


// 它是玩家登录流程的 “最早可定制节点”，甚至早于 PostLogin；
// 核心价值是解析登录 URL 参数，实现 “带参数登录” 的定制化（比如指定玩家队伍、皮肤、出生点）；
// 它不负责 “通用初始化”（这是 GenericPlayerInitialization 的活），只负责 “登录时的参数解析和玩家定制”。
FString AMyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
    const FString& Options, const FString& Portal)
{
    return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AMyGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
    Super::InitSeamlessTravelPlayer(NewController);
}
