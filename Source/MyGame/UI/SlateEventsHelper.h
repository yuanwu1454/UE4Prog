#pragma once

#include "CoreMinimal.h"
#include "MyInputProcessor.h"
#include "Framework/Application/SlateApplication.h"


class FSlateEventsHelper
{
public:
	static FSlateEventsHelper& Get();

	void Initialize();
	void Shutdown();
	TSharedPtr<FMyInputProcessor> GetMyInputProcessor();


private:
	void RegularQuerySlateApplicationExample();
	void RegularUseSlateApplicationExample();
	FSlateEventsHelper() = default;
	~FSlateEventsHelper() = default;

	// 所有监听句柄
	FDelegateHandle PreTickHandle;
	FDelegateHandle PostTickHandle;
	FDelegateHandle PreShutdownHandle;
	FDelegateHandle UserRegisteredHandle;
	FDelegateHandle WindowBeingDestroyedHandle;
	FDelegateHandle FocusChangingHandle;
	FDelegateHandle ApplicationActivationHandle;
	FDelegateHandle ModalLoopTickHandle;
	FDelegateHandle LastUserInteractionUpdateHandle;
	FDelegateHandle WindowActionHandle;

#if WITH_EDITOR
	FDelegateHandle PreInputKeyDownHandle;
	FDelegateHandle PreMouseButtonDownHandle;
	FDelegateHandle WindowDPIScaleChangedHandle;
	FDelegateHandle SystemSignalDPIChangedHandle;
#endif
	TSharedPtr<FMyInputProcessor> MyInputProcessor;
	// 回调函数
	void OnPreTick(float DeltaTime);
	void OnPostTick(float DeltaTime);
	void OnPreShutdown();
	void OnUserRegistered(int32 UserId);
	void OnWindowBeingDestroyed(const SWindow& Window);
	void OnFocusChanging(const FFocusEvent&, const FWeakWidgetPath&, const TSharedPtr<SWidget>&, const FWidgetPath&, const TSharedPtr<SWidget>&);
	void OnApplicationActivationChanged(bool IsActive);
	void OnModalLoopTick(float DeltaTime);
	void OnLastUserInteractionTimeUpdate(double NewTime);
	bool OnWindowAction(const TSharedRef<FGenericWindow>&, EWindowAction::Type);

#if WITH_EDITOR
	void OnApplicationPreInputKeyDown(const FKeyEvent& KeyEvent);
	void OnApplicationMousePreInputButtonDown(const FPointerEvent& MouseEvent);
	void OnWindowDPIScaleChanged(TSharedRef<SWindow> Window);
	void OnSystemSignalDPIChanged(TSharedRef<SWindow> Window);
#endif
};