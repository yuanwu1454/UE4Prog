#pragma once
#include "CoreMinimal.h"
#include "SlateCore.h"

UENUM(BlueprintType)
enum class EUILayer : uint8
{
	Background,
	Normal,
	Popup,
	Top
};

UENUM(BlueprintType)
enum class EUILifeState : uint8
{
	None,
	Initialized,
	Visible,
	Close
};
// UI 的业务层次与物理层次
// 二、为什么大厂 / 规范架构非要加「逻辑 ParentPage」？
// 1. 物理层级 ≠ 业务层级，必须做两层隔离
// 举个真实场景：
// 你游戏有：
// 主界面 PageA（全屏）
// 弹出商城 Panel1
// 再弹出详情 Panel2
// 再弹出充值弹窗 Panel3
// 物理 UMG 层级：
// Root→Panel1→Panel2→Panel3
// 但业务逻辑层级应该是：
// 所有 Panel1/2/3 都归属主界面 PageA
// 如果你只靠 AddChild 挂 UI：
// 你关掉 PageA 的时候 → 你怎么知道要连带删干净所有嵌套 Panel？
// 你遍历 UI 树找子面板？性能烂、还容易漏、还会误删其他页面弹窗
// 有逻辑 ParentPage：
// PageA.Close () → 自动遍历自己所有逻辑子 Panel → 全删、全停音效、全停动画、全清内存
// 和物理 UI 树无关，纯业务托管。
// 2. 弹窗优先级、遮挡屏蔽、输入拦截，必须靠逻辑父级
// 游戏刚需：
// 顶层 Panel 拦截所有底层按键
// 一个 Page 全屏时，下级 Page 自动隐藏
// 同一个 Page 下所有 Panel 统一灰化 / 禁交互
// 只靠 UG 物理层级：
// 你要递归遍历 Widget 树、查 Render 层级、查 ZOrder，巨麻烦还不稳定。
// 有 ParentPage 逻辑绑定：
// 一行代码：ParentPage->GetAllChildPanels() 直接管控。
// 3. 内存回收、Wwise 音效、动画自动清理，必须靠父级归属
// 你原来代码里有：
// 一堆 AkEvent 播放
// 一堆动画 Play
// 一堆 PlayingID
// 如果 Panel 是游离 AddChild：
// Panel 隐藏了、销毁了，音效还在播、动画还在跑、内存还挂着。
// 有逻辑 Parent：
// Page 销毁 → 统一触发所有子 Panel OnClose → 自动 StopAllSound/StopAllAnim
// 不会漏、不会炸、不会留幽灵音效。
// 4. 多页面切换、堆栈管理，没有父级直接写死
// 比如：
// 打开背包 Page → 弹出道具 Panel → 切回主 Page
// 你要：
// 背包 Page 隐藏 → 所有道具 Panel 一起隐藏
// 纯 AddChild：引擎不知道这个 Panel 属于背包，你得手动一个个藏。
// 5. 解耦通信：Panel 不能到处拿全局，只能找爹
// Panel 内部要调页面功能（刷新列表、提交数据）：
// 不做 Parent：只能全局单例拿 ViewManager → 代码到处耦合
// 有 Parent：GetParentPage()->RefreshData() 干净、收敛、好维护