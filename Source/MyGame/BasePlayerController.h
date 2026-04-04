// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyGame/Router/Room/RoomRouter.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ABasePlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void OnApplicationActivationStateChanged(bool bActive);
	// ========== Router 访问器（对外唯一接口） ==========
	UFUNCTION(BlueprintCallable, Category = "Router")
	URoomRouter* GetRoomRouter() const { return RoomRouter; }
	

	// ========== 通用回调（所有 Router 共享） ==========
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestFailed, const FString&, Reason);
	UPROPERTY(BlueprintAssignable, Category = "Request")
	FOnRequestFailed OnRequestFailed;

	// 权限判断（所有 Router 共享）
	UFUNCTION(BlueprintCallable, Category = "Permission")
	bool IsAdmin() const { return bIsAdmin; }

	// 切换图元组件渲染开关（带安全检查）
	UFUNCTION(BlueprintCallable, Category = "Rendering")
	void TogglePrimitiveComponentRendering(bool bEnableRender);

	// 获取当前渲染开关状态
	UFUNCTION(BlueprintPure, Category = "Rendering")
	bool IsPrimitiveComponentRenderingEnabled() const { return bRenderPrimitiveComponents; }

	
public:
	// 重写引擎的虚函数，自定义隐藏Actor逻辑
	virtual void UpdateHiddenActors(const FVector& ViewLocation) override;

private:
	// 辅助方法：检测相机是否穿透某个Actor
	bool IsCameraPenetratingActor(AActor* Actor, const FVector& CameraLocation);

	
public:
	/**
	 * Builds a list of components that are hidden based upon gameplay
	 * 构建基于游戏玩法需要隐藏的组件列表
	 * @param ViewLocation the view point to hide/unhide from
	 *        用于判断隐藏/显示的相机视角位置
	 * @param HiddenComponents the list to add to/remove from
	 *        要添加/移除隐藏组件的目标列表（渲染层最终使用的列表）
 	 */
	// 重写组件级隐藏逻辑
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& HiddenComponents) override;

	// 切换光标样式（蓝图可调用）
	UFUNCTION(BlueprintCallable, Category = "Cursor")
	void SwitchCursor(EMouseCursor::Type CursorType, TSubclassOf<UUserWidget> CursorWidgetClass);
	/** Pawn has been possessed, so changing state to NAME_Playing. Start it walking and begin playing with it. */
	
	virtual void BeginPlayingState() override;

	/** Leave playing state. */ 
	virtual void EndPlayingState() override;
private:
	// 辅助方法：获取带指定标签的所有PrimitiveComponent
	void GetComponentsWithTag(FName Tag, TArray<UPrimitiveComponent*>& OutComponents);

	// 可选：根据玩家类型动态调整带宽（比如VIP玩家更高上限）
	UFUNCTION(BlueprintCallable, Category = "Network")
	void SetDynamicClientCap(int32 NewCap);


	
	virtual void DelayedPrepareMapChange() override;

	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override;


	virtual void SeamlessTravelTo(class APlayerController* NewPC) override;
	
	virtual void SeamlessTravelFrom(class APlayerController* OldPC) override;

	virtual void PostSeamlessTravel()override;

	virtual void BuildInputStack(TArray<UInputComponent*>& InputStack) override;

	virtual void SendToConsole(const FString& Command) override;
	void GetLocalPlayerViewport();

	bool IsLocalPlayerController();
	bool IsNetPlayerController();
	
	virtual void NotifyLoadedWorld(FName WorldPackageName, bool bFinalDest) override;

private:
	// ========== 各类 Router 实例（按业务归类） ==========
	UPROPERTY()
	URoomRouter* RoomRouter;

	

	// 玩家权限标识
	UPROPERTY()
	bool bIsAdmin = false;
	// 保存自定义光标Widget实例（避免重复创建）
	UPROPERTY()
	UUserWidget* CustomCursorWidget;
	FDelegateHandle AppActivatedHandle;
	void ForceCaptureMouse();
};

// Actor 没有 “原生隐藏属性”，其 “隐藏” 本质是批量控制旗下所有 PrimitiveComponent 的渲染状态；
// SetActorHiddenInGame是引擎提供的 “快捷方式”，帮你批量修改组件状态 + 同步到HiddenActors列表；
// 引擎最终的隐藏逻辑（BuildHiddenComponentList），是通过识别组件的渲染状态、并将其加入渲染层的隐藏列表来实现的。
//
// UInputComponent 是 “输入响应的执行者”（绑定按键、处理输入），
// 而 UPlayerInput 是 “输入调度的总管家”—— 它负责接收底层硬件输入（键盘、鼠标、手柄），
// 并按照输入栈优先级分发给对应的 UInputComponent，是连接硬件输入和逻辑响应的关键桥梁。

// K2_ClientPlayForceFeedback
// APlayerController 的客户端执行函数，用于在玩家的手柄 / 外设上触发震动效果（比如开枪后手柄震动、被击中时的震动反馈
// ClientStopForceFeedback
// 停止所有力反馈

// 只有客户端能调用服务器 RPC（server 标记的函数）；
// 只有服务器能调用客户端 RPC（client 标记的函数）；
// 用 HasAuthority() 判断当前是否在服务器端（拥有网络权限）

// NetPlayerIndex
// NetPlayerIndex 是分屏游戏的核心索引，用于标识同一设备 / 连接下的不同分屏玩家，从 0 开始编号；
// 核心作用是让 UE4 网络代码匹配 PlayerController 与对应的分屏视口、输入、子连接，避免分屏数据错乱；
// 开发中可主动利用它做分屏玩家的差异化处理（如不同视口、不同输入绑定），但禁止手动修改该值。

// PendingSwapConnection
// 场景
// 分屏玩家切换角色 / 控制权；
// 玩家断线重连后恢复控制权；
// 管理员强制切换玩家的控制对象；
// 多人游戏中 “替补玩家” 接管控制权。
// 作用 标记旧 PC 需要等待哪个UNetConnection（网络连接）的确认，才能销毁
// 简单来讲就是告知PC 替换完成 要记得及时清理老pc的作用

// UNetConnection* NetConnection
// UE4 网络层的核心类，代表一个端到端的网络连接（如客户端↔服务器的 TCP/UDP 连接）
// 每个 PC（玩家控制器）都会通过这个指针 “绑定” 它对应的网络连接 —— 无论是客户端的 PC，
// 还是服务器上代表远程玩家的 PC，NetConnection 就是 PC 和网络层之间的 “桥梁”，
// 负责传递该玩家的所有网络数据（输入、同步、RPC 等）。
// 其实就是网络连接接口

// DuplicateTransient
// 是 UPROPERTY 的属性标记（Specifier），专门用于控制 UE4 的Actor / 对象复制（Duplicate）行为
// 通过 “复制 Actor”（比如 Ctrl+W 复制、代码中调用DuplicateObject/DuplicateActor）时，该属性的值不会被复制到新对象中，
// 而是保留新对象的默认值（通常是nullptr/0 / 空），核心作用是避免 “复制对象时携带无效 / 冲突的临时数据”。

// Transient
// 明确告诉 UE4：不要序列化这个属性（优先级最高）；
// 无 Transient = 让 UE4 根据默认规则判断：是否需要序列化（可能序列化，也可能不序列化）。

// RotationInput
// 定义旋转控制、修改旋转灵敏度、限制旋转范围：
// RotationInput 是 PlayerController 中每帧累计旋转输入的临时缓冲区，用 FRotator 存储 Pitch（俯仰）、Yaw（偏航）、Roll（翻滚）的增量值；
// 核心流程：每帧累加旋转输入 → 处理旋转（更新视角 / 角色朝向） → 清空该变量；

// PlayerTick
// 场景 1：基础重写（保留引擎默认逻辑 + 自定义扩展）
// 场景 2：完全自定义 PlayerTick（替换默认旋转 / 移动逻辑）

// PreProcessInput 和 PostProcessInput
// 是玩家控制器（APlayerController）输入处理流程的 “前置 / 后置钩子函数”，你可以把它们理解为：
// 输入处理的 “预处理 - 主处理 - 后处理” 三段式流程中，
// PreProcessInput 是 “输入处理前的准备工作”，PostProcessInput 是 “输入处理后的收尾工作”——
// 它们围绕 PlayerTick（输入主处理）展开，用于在不修改核心输入逻辑的前提下，灵活扩展输入相关的预处理 / 后处理逻辑。

// GetHitResultAtScreenPosition
// 点击拾取物体

// ProjectWorldLocationToScreen
// 就是用来判断是否在视野外的

// PostProcessWorldToScreen 是 UE4/UE5 中 PlayerController 类的虚函数，作用是在 ProjectWorldLocationToScreen 完成 “3D 世界坐标→2D 屏幕坐标” 的投射后，对结果做自定义的后置处理，并最终决定这个投射坐标是否依然有效。
// 场景 1：给屏幕坐标加固定偏移（比如 UI 图标偏移）
// 场景 2：强制屏蔽特定区域的投射结果
// PostProcessWorldToScreen 是投射后的自定义扩展钩子，用于修改投射结果或判定结果有效性；

// UpdateRotation(float DeltaTime)
// 场景 1：限制玩家视角旋转范围（比如固定视角游戏）
// 场景 2：自定义旋转速度（比如慢动作视角）

// InputYawScale/InputPitchScale/InputRollScale
// 是视角旋转的灵敏度缩放系数，

//exec
// 加了exec的函数 = 代码调用 + 控制台快捷调用，是 UE 调试流程中 “提效神器”

// ClientAddTextureStreamingLoc
// 是服务器向客户端发送的可靠 RPC，核心作用是指定纹理流送的优先级位置，让客户端优先加载该区域高分辨率纹理；
// 为啥传位置就能知道纹理呢？
// 所有需要流送的纹理，都能通过绑定的资源，找到自己在世界中的 “空间范围” —— 这是 “传位置就能关联纹理” 的基础。
// 位置是 “搜索中心”，引擎按 “距离 + 资源范围” 搜索该中心周围的纹理，再决定加载哪张、加载多高清 —— 这就是 “传位置就能控制纹理加载” 的本质
// 持续时间 Duration 引擎以这个位置为核心优先加载高分辨率纹理” 的 “有效期”—— 超时后，引擎会自动取消这个位置的优先级，恢复到默认的纹理流送逻辑（以玩家当前视角位置为核心）。

// 都是服务器通知客户端
// ClientCommitMapChange()：执行由 PrepareMapChange() 准备好的地图切换（真正触发关卡加载 / 跳转）；
// ClientCancelPendingMapChange()：取消客户端正在等待的地图切换（终止已准备但未执行的关卡跳转）。
// ClientForceGarbageCollection() 强制客户端在当前 Tick 结束时执行一次垃圾回收（Garbage Collection，简称 GC）
// ClientFlushLevelStreaming() 指令客户端 “阻塞等待” 所有待处理的关卡流送（Level Streaming）操作完成
// 这个函数的设计初衷是解决 “玩家加入游戏时关卡流送不同步” 的问题：比如新玩家加入联网游戏时，客户端可能有多个关卡还在 “待加载” 状态，
// 调用该函数能强制客户端立刻完成这些流送操作，让玩家快速看到完整的游戏场景，而非 “加载一半的残缺场景”。
//
// ClientIgnoreLookInput(bool bIgnore)：指令客户端忽略 / 恢复视角控制输入（比如鼠标转动视角、手柄右摇杆操作）；
// ClientIgnoreMoveInput(bool bIgnore)：指令客户端忽略 / 恢复移动控制输入（比如 WASD、手柄左摇杆、跳跃 / 冲刺）。
// 场景 1：剧情播放时禁用所有输入，剧情结束后恢复
// 场景 2：加载关卡时禁用移动，加载完成后恢复

// ClientMessage(const FString& S, FName Type = NAME_None, float MsgLifeTime = 0.f);
// 服务器指令客户端在 HUD（游戏界面）上显示指定的文本消息，并可自定义消息类型、显示时长

// ClientPlayCameraAnim(class UCameraAnim* AnimToPlay, float Scale=1.f, float Rate=1.f, float BlendInTime=0.f, float BlendOutTime=0.f, bool bLoop=false, bool bRandomStartTime=false, ECameraShakePlaySpace Space=ECameraShakePlaySpace::CameraLocal, FRotator CustomPlaySpace=FRotator::ZeroRotator);
// 指令客户端在当前玩家相机上播放指定的相机动画（CameraAnim） —— 比如镜头晃动、震动、缩放、旋转等视觉反馈（如开枪后镜头后坐力、爆炸时镜头抖动、剧情中镜头推拉），是实现 “相机动效” 的核心接口。
// 场景 1：开枪后播放相机后坐力动画（服务器指令）
// 场景 2：爆炸时播放镜头震动动画（全局推送）

// ClientPlaySound 播放声音

// ClientPrepareMapChange
// 核心作用是：服务器分批次指令客户端异步加载指定关卡资源，为后续的流式地图切换（Streaming Map Transition）做准备。
// 这个函数的设计背景是：UE 的 RPC 不支持直接复制动态数组（比如一次性传多个关卡名），因此服务器需要通过 “多次调用该函数（每调用一次传一个关卡名）” 的方式，分批次告知客户端需要加载的所有关卡；同时通过 bFirst/bLast 标记批次的开始 / 结束，让客户端知道何时清空旧列表、何时开始正式准备切换。

// UPlayer* Player 是 APlayerController 中的核心属性，用于关联玩家对象（UPlayer）—— 这个对象是 UE 中连接「玩家控制器（PlayerController）」和「游戏视口 / 输入系统」的桥梁，简单说：
// APlayerController 负责处理游戏逻辑（如输入响应、角色控制）；
// UPlayer 负责底层的「玩家身份」管理（如本地玩家 / 联网玩家、视口绑定、输入路由）；
// 二者通过这个 Player 属性关联，确保输入能准确传递到对应的 PlayerController。

// APawn* AcknowledgedPawn 是 APlayerController 中用于联网游戏的核心属性，作用是：
// 客户端在「拥有（Possess）某个 Pawn」后，向服务器确认 “已成功接管该 Pawn”，服务器通过这个属性记录「客户端已确认拥有的 Pawn」，确保联网场景下 PlayerController 和 Pawn 的绑定关系在网络两端同步一致。
// 简单说：
// 单机游戏中这个属性几乎没用；
// 联网游戏中，它是 “客户端告知服务器：我已经成功控制这个 Pawn 了” 的 “确认标记”，避免网络延迟导致的控制不同步。
// AcknowledgedPawn 在所有联网角色（客户端、监听服务器、专用服务器） 上都存在，只是：
// 客户端：主动设置该值，并向服务器发送确认；
// 服务器（DS / 监听）：接收客户端的确认后同步设置该值，用于记录 “客户端已确认拥有的 Pawn”。

// AHUD 是传统的 “即时绘制” HUD（通过DrawHUD()方法绘制 2D 图形 / 文字）；
// UMG 是现代 UI 系统，通常会在 HUD 中创建 UMG 控件（MyHUD中持有 UMG 引用），而非直接用DrawHUD()。
// MyHUD 是 PlayerController 关联专属 HUD 实例的核心指针，每个玩家独立持有，负责渲染该玩家视角的实时 UI；
// 无需手动赋值，引擎自动初始化，使用前需做空指针检查并转换为自定义 HUD 类；
// 联网场景下仅客户端的MyHUD负责绘制，服务器端MyHUD仅作为对象实例存在，无渲染作用。

// bAutoManageActiveCameraTarget
// 这是 UE 玩家控制器（PlayerController）中控制相机目标自动管理的核心布尔属性，决定了相机目标的控制权是交给引擎自动处理，还是由你手动接管。
// 自动管理（true）：引擎会自动将相机目标绑定到玩家当前控制的 Pawn（比如玩家角色），相机始终跟随这个 Pawn 移动 / 旋转
// 手动管理（false）：引擎不再自动更新相机目标，你需要自己指定相机该看向哪里、跟踪哪个对象


// BlendedTargetViewRotation
// 本地玩家的视角旋转是实时更新的，不会有问题
// 其他玩家的视角旋转需要通过网络从服务器同步到本地客户端
// 网络数据包不是连续的（有延迟 / 间隔），如果直接使用同步过来的 TargetViewRotation，会导致其他玩家的视角 “一跳一跳” 地变化（抖动）
// BlendedTargetViewRotation 就是为了解决这个问题，通过插值平滑让旋转变化更自然



// void AMyCharacter::Tick(float DeltaTime)
// {
// 	APlayerController* PC = Cast<APlayerController>(Controller);
// 	if (PC && PC->WasInputKeyJustPressed(EKeys::Q) && !bSkillOnCooldown)
// 	{
// 		CastSkill(); // 仅按下Q且技能冷却完成时释放一次
// 		StartSkillCooldown();
// 	}
// }
// WasInputKeyJustPressed	仅按键按下的那一帧	跳跃、开火、交互、确认	移动（按住 W 只走一步）
// IsInputKeyDown	按键持续按住的每一秒	移动、加速、持续瞄准	跳跃（按住空格狂跳）

//
// APlayerController 的函数可归纳为 8 大核心类别，核心逻辑围绕：
// 1. 生命周期与状态：管理自身状态切换（游玩 / 观战 / 非活跃）；
// 2. Pawn 控制：拥有 / 释放 / 重启 Pawn，是玩家控制角色的核心；
// 3. 输入与交互：处理输入、分发输入事件、控制输入模式；
// 4. 视角与相机：管理相机视角、同步视角状态、处理视觉反馈；
// 5. 网络同步：适配多人游戏的客户端 - 服务端状态同步；
// 6. UI 与反馈：HUD、鼠标、震动 / 音效等玩家交互反馈；
// 7. 观战逻辑：专门适配观战模式的视角 / 状态管理；
// 8. 工具函数：坐标转换、状态查询等通用辅助逻辑。

// exec 函数
// ServerExec，RestartLevel，FOV，Pause，SetName,SwitchLevel

//  SetPause() 是 PlayerController 的虚函数，核心功能是：
// 本地暂停游戏（单机 / 客户端），如果是联网游戏会自动调用 ServerPause() 同步到服务端；
// 最终会调用 GameModeBase::SetPause() 完成实际的暂停逻辑；
// 返回值 bool 表示暂停 / 恢复操作是否成功（比如游戏已暂停时再次调用暂停会返回 false）。

// void ServerUpdateLevelVisibility(const FUpdateLevelVisibilityLevelInfo& LevelVisibility);
// 用于客户端告知服务端 “我加载 / 卸载了某个流送关卡”，从而保证服务端只同步客户端可见的 Actor（避免同步客户端未加载的 Actor 导致出错）
// 监听关卡可见性变化（扩展业务逻辑）
// ServerUpdateLevelVisibility() 是客户端→服务端的可靠 RPC，引擎自动调用，用于同步流送关卡的可见性状态；

// ServerUpdateMultipleLevelsVisibility（批量版）
// 无论变更多少关卡，只发 1 次 RPC

// SealedEvent	密封事件：禁止子类重写这个函数（只能重写验证函数或依赖引擎回调）

// ServerVerifyViewTarget 可以重写validation 与implemention
// 用于客户端向服务端 “确认当前视角目标”，保证联网场景下客户端和服务端的视角目标一致

// ServerViewPrevPlayer，ServerViewNextPlayer， ServerViewSelf
// 移动视角 可以重写相关函数

// AddYawInput,AddPitchInput,AddRollInput

// SendClientAdjustment ：让服务器向客户端的 Pawn 同步调整数据（比如位置、状态、属性等）

// PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel);
// 专门处理客户端地图 / 服务器跳转前的预处理逻辑：
