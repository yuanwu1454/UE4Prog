#include "SlateEventsHelper.h"

FSlateEventsHelper& FSlateEventsHelper::Get()
{
	static FSlateEventsHelper Instance;
	return Instance;
}

void FSlateEventsHelper::Initialize()
{
	FSlateApplication& SlateApp = FSlateApplication::Get();

	// 每帧
	PreTickHandle = SlateApp.OnPreTick().AddRaw(this, &FSlateEventsHelper::OnPreTick);
	PostTickHandle = SlateApp.OnPostTick().AddRaw(this, &FSlateEventsHelper::OnPostTick);

	// 关闭
	PreShutdownHandle = SlateApp.OnPreShutdown().AddRaw(this, &FSlateEventsHelper::OnPreShutdown);

	// 用户
	UserRegisteredHandle = SlateApp.OnUserRegistered().AddRaw(this, &FSlateEventsHelper::OnUserRegistered);

	// 窗口
	WindowBeingDestroyedHandle = SlateApp.OnWindowBeingDestroyed().AddRaw(this, &FSlateEventsHelper::OnWindowBeingDestroyed);

	// 焦点
	FocusChangingHandle = SlateApp.OnFocusChanging().AddRaw(this, &FSlateEventsHelper::OnFocusChanging);

	// 应用激活/失活
	ApplicationActivationHandle = SlateApp.OnApplicationActivationStateChanged().AddRaw(this, &FSlateEventsHelper::OnApplicationActivationChanged);

	// 模态窗口Tick
	ModalLoopTickHandle = SlateApp.GetOnModalLoopTickEvent().AddRaw(this, &FSlateEventsHelper::OnModalLoopTick);

	// 用户交互时间更新
	LastUserInteractionUpdateHandle = SlateApp.GetLastUserInteractionTimeUpdateEvent().AddRaw(this, &FSlateEventsHelper::OnLastUserInteractionTimeUpdate);

	// 窗口动作（最大化、最小化、关闭等）
	WindowActionHandle = SlateApp.RegisterOnWindowActionNotification(
		FOnWindowAction::CreateRaw(this, &FSlateEventsHelper::OnWindowAction)
	);

#if WITH_EDITOR
	// 编辑器全局输入监听
	PreInputKeyDownHandle = SlateApp.OnApplicationPreInputKeyDownListener().AddRaw(this, &FSlateEventsHelper::OnApplicationPreInputKeyDown);
	PreMouseButtonDownHandle = SlateApp.OnApplicationMousePreInputButtonDownListener().AddRaw(this, &FSlateEventsHelper::OnApplicationMousePreInputButtonDown);

	// DPI
	WindowDPIScaleChangedHandle = SlateApp.OnWindowDPIScaleChanged().AddRaw(this, &FSlateEventsHelper::OnWindowDPIScaleChanged);
	SystemSignalDPIChangedHandle = SlateApp.OnSystemSignalsDPIChanged().AddRaw(this, &FSlateEventsHelper::OnSystemSignalDPIChanged);
#endif

	UE_LOG(LogTemp, Log, TEXT("✅ SlateEventsHelper 全部监听已绑定"));

	// 注册
	MyInputProcessor = MakeShareable(new FMyInputProcessor());
	SlateApp.RegisterInputPreProcessor(MyInputProcessor);
}

void FSlateEventsHelper::Shutdown()
{
	FSlateApplication& SlateApp = FSlateApplication::Get();

	SlateApp.OnPreTick().Remove(PreTickHandle);
	SlateApp.OnPostTick().Remove(PostTickHandle);
	SlateApp.OnPreShutdown().Remove(PreShutdownHandle);
	SlateApp.OnUserRegistered().Remove(UserRegisteredHandle);
	SlateApp.OnWindowBeingDestroyed().Remove(WindowBeingDestroyedHandle);
	SlateApp.OnFocusChanging().Remove(FocusChangingHandle);
	SlateApp.OnApplicationActivationStateChanged().Remove(ApplicationActivationHandle);
	SlateApp.GetOnModalLoopTickEvent().Remove(ModalLoopTickHandle);
	SlateApp.GetLastUserInteractionTimeUpdateEvent().Remove(LastUserInteractionUpdateHandle);
	SlateApp.UnregisterOnWindowActionNotification(WindowActionHandle);

#if WITH_EDITOR
	SlateApp.OnApplicationPreInputKeyDownListener().Remove(PreInputKeyDownHandle);
	SlateApp.OnApplicationMousePreInputButtonDownListener().Remove(PreMouseButtonDownHandle);
	SlateApp.OnWindowDPIScaleChanged().Remove(WindowDPIScaleChangedHandle);
	SlateApp.OnSystemSignalsDPIChanged().Remove(SystemSignalDPIChangedHandle);
#endif

	UE_LOG(LogTemp, Log, TEXT("✅ SlateEventsHelper 全部监听已移除"));
	
	// 注销
	SlateApp.UnregisterInputPreProcessor(MyInputProcessor);
}

void FSlateEventsHelper::RegularQuerySlateApplicationExample()
{
	// 常用查询接口（高频）
	// FSlateApplication& SlateApp = FSlateApplication::Get();
	//
	// // 1. 应用状态
	// bool bIsActive = SlateApp.IsActive(); // 应用是否前台激活
	// bool bHasModalWindow = SlateApp.GetActiveModalWindow().IsValid(); // 是否有模态窗口
	// bool bAnyMenusVisible = SlateApp.AnyMenusVisible(); // 是否有菜单打开
	// bool bIsDragDropping = SlateApp.IsDragDropping(); // 是否正在拖放
	//
	// // 2. 输入/设备状态
	// FModifierKeysState Modifiers = SlateApp.GetModifierKeys(); // Ctrl/Shift/Alt状态
	// bool bIsMouseAttached = SlateApp.IsMouseAttached(); // 是否有鼠标
	// bool bIsGamepadAttached = SlateApp.IsGamepadAttached(); // 是否有手柄
	// FVector2D CursorPos = SlateApp.GetCursorPos(); // 当前鼠标位置
	// float DeltaTime = SlateApp.GetDeltaTime(); // 帧间隔时间
	//
	// // 3. 窗口/视图相关
	// TSharedPtr<SWindow> ActiveWindow = SlateApp.GetActiveTopLevelWindow(); // 活跃顶层窗口
	// TSharedPtr<SViewport> GameViewport = SlateApp.GetGameViewport(); // 游戏视口
	// TArray<TSharedRef<SWindow>> AllWindows = SlateApp.GetTopLevelWindows(); // 所有顶层窗口
	//
	// // 4. 焦点相关
	// TSharedPtr<SWidget> FocusedWidget = SlateApp.GetKeyboardFocusedWidget(); // 键盘焦点Widget
	// TSharedPtr<SWidget> UserFocused = SlateApp.GetUserFocusedWidget(0); // 指定用户焦点
}

void FSlateEventsHelper::RegularUseSlateApplicationExample()
{
	// 第三类：全局操作接口（业务主动控制 UI，非监听）
	// FSlateApplication& SlateApp = FSlateApplication::Get();
	// TSharedPtr<SWidget> MyWidget;
	// // 1. 焦点控制（全局设置/清除焦点）
	// SlateApp.SetUserFocus(0, MyWidget); // 给用户0设置焦点到MyWidget
	// SlateApp.ClearUserFocus(0); // 清除用户0焦点
	// SlateApp.SetAllUserFocus(MyWidget); // 所有用户焦点
	// SlateApp.SetKeyboardFocus(MyWidget); // 设置键盘焦点
	//
	// // 2. 鼠标/光标控制
	// SlateApp.SetCursorPos(FVector2D(100, 100)); // 强制设置鼠标位置
	// SlateApp.SetPlatformCursorVisibility(false); // 隐藏系统光标
	// SlateApp.ReleaseAllPointerCapture(); // 释放所有鼠标捕获
	//
	// // 3. 窗口控制（主动创建/管理窗口）
	// TSharedRef<SWindow> NewWindow = SNew(SWindow).Title(TEXT("MyWindow"));
	// SlateApp.AddWindow(NewWindow, true); // 添加并显示窗口
	// SlateApp.RequestDestroyWindow(NewWindow); // 安全销毁窗口
	// SlateApp.ForceRedrawWindow(NewWindow); // 强制重绘窗口
	//
	// // 4. 菜单/提示框控制
	// SlateApp.DismissAllMenus(); // 关闭所有菜单\
	//
	// // SlateApp.SpawnToolTip(MyToolTipWidget, FVector2D(200,200)); // 显示ToolTip
	// SlateApp.CloseToolTip(); // 关闭ToolTip
	//
	// // 5. 输入重置（异常恢复）
	// SlateApp.ResetToDefaultInputSettings(); // 重置所有输入状态（捕获、焦点、光标）


	// 全局配置 / 样式 / 输入控制（业务定制 UI 行为）
	// 1. 应用缩放（高DPI适配）
	// SlateApp.SetApplicationScale(1.0f); // 设置全局缩放比例
	// float CurrentScale = SlateApp.GetApplicationScale();
	//
	// // 2. 菜单/动画控制
	// SlateApp.EnableMenuAnimations(true); // 开启/关闭菜单动画
	// bool bAnimEnabled = SlateApp.AreMenuAnimationsEnabled();
	//
	// // 3. 导航配置（手柄/键盘导航）
	// TSharedRef<FNavigationConfig> MyNavConfig = MakeShareable(new FNavigationConfig());
	// SlateApp.SetNavigationConfig(MyNavConfig); // 自定义导航规则
	//
	// // 4. 输入映射（多用户/控制器映射）
	// TSharedRef<ISlateInputManager> MyInputManager = MakeShareable(new FSlateDefaultInputMapping());
	// SlateApp.SetInputManager(MyInputManager); // 自定义输入到用户的映射
	//
	// // 5. 光标/拖拽配置
	// SlateApp.SetCursorRadius(2.0f); // 光标点击半径
	// SlateApp.SetDragTriggerDistance(5.0f); // 拖拽触发距离（像素）
	

	// 第六类：工具 / 辅助功能（业务开发 / 调试）

	// 1. 截图（Widget/窗口截图）
	// TArray<FColor> ScreenshotData;
	// FIntVector Size;
	// SlateApp.TakeScreenshot(MyWidget, ScreenshotData, Size);

	// 2. 路径生成（查找Widget路径）
	// FWidgetPath WidgetPath;
	// SlateApp.GeneratePathToWidgetUnchecked(MyWidget, WidgetPath);

	// 3. 弹出支持（点击外部关闭菜单/提示）
	// FPopupSupport& PopupSupport = SlateApp.GetPopupSupport();
	// 注册点击外部通知
	// PopupSupport.RegisterClickNotification(MyWidget, FOnClickedOutside::CreateLambda([]() {
	// 	UE_LOG(LogTemp, Log, TEXT("点击了MyWidget外部"));
	// }));

	// 六、第五类：输入预处理器（比监听更底层，可拦截输入）
	
}

// ------------------------------------------------------
// 实现
// ------------------------------------------------------

void FSlateEventsHelper::OnPreTick(float DeltaTime)
{
	// UE_LOG(LogTemp, Log, TEXT("PreTick: %.2f"), DeltaTime);
}

void FSlateEventsHelper::OnPostTick(float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("PostTick: %.2f"), DeltaTime);
}

void FSlateEventsHelper::OnPreShutdown()
{
	UE_LOG(LogTemp, Log, TEXT("PreShutdown"));
}

void FSlateEventsHelper::OnUserRegistered(int32 UserId)
{
	UE_LOG(LogTemp, Log, TEXT("UserRegistered: %d"), UserId);
}

void FSlateEventsHelper::OnWindowBeingDestroyed(const SWindow& Window)
{
	UE_LOG(LogTemp, Log, TEXT("WindowBeingDestroyed %s"), *Window.GetTitle().ToString());
}

void FSlateEventsHelper::OnFocusChanging(const FFocusEvent&, const FWeakWidgetPath&, const TSharedPtr<SWidget>&, const FWidgetPath&, const TSharedPtr<SWidget>&)
{
	UE_LOG(LogTemp, Log, TEXT("FocusChanging"));
}

void FSlateEventsHelper::OnApplicationActivationChanged(bool IsActive)
{
	UE_LOG(LogTemp, Log, TEXT("ApplicationActivationChanged: %d"), IsActive);
}

void FSlateEventsHelper::OnModalLoopTick(float DeltaTime)
{
	// UE_LOG(LogTemp, Log, TEXT("ModalLoopTick: %.2f"), DeltaTime);
}

void FSlateEventsHelper::OnLastUserInteractionTimeUpdate(double NewTime)
{
	UE_LOG(LogTemp, Log, TEXT("LastUserInteractionTimeUpdate: %.2f"), NewTime);
}

bool FSlateEventsHelper::OnWindowAction(const TSharedRef<FGenericWindow>& Win, EWindowAction::Type Act)
{
	UE_LOG(LogTemp, Log, TEXT("WindowAction:Act %d"), Act);
	return false;
}

#if WITH_EDITOR
void FSlateEventsHelper::OnApplicationPreInputKeyDown(const FKeyEvent& KeyEvent)
{
	UE_LOG(LogTemp, Log, TEXT("ApplicationPreInputKeyDown: %s"), *KeyEvent.GetKey().ToString());
}

void FSlateEventsHelper::OnApplicationMousePreInputButtonDown(const FPointerEvent& MouseEvent)
{
	UE_LOG(LogTemp, Log, TEXT("ApplicationMousePreInputButtonDown: %s"), *MouseEvent.GetEffectingButton().ToString());
}

void FSlateEventsHelper::OnWindowDPIScaleChanged(TSharedRef<SWindow> Window)
{
	UE_LOG(LogTemp, Log, TEXT("WindowDPIScaleChanged: %s"), *Window->GetTitle().ToString());
}

void FSlateEventsHelper::OnSystemSignalDPIChanged(TSharedRef<SWindow> Window)
{
	UE_LOG(LogTemp, Log, TEXT("SystemSignalDPIChanged: %s"), *Window->GetTitle().ToString());
}
#endif
