#pragma once

#include "CoreMinimal.h"

#include "Framework/Application/IInputProcessor.h"


class FMyInputProcessor : public IInputProcessor
{
public:
	FMyInputProcessor();
	~FMyInputProcessor();
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;

	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& WheelEvent, const FPointerEvent* GestureEvent) override;
	void SetViewManager(class UMyViewManager* InViewManager);

private:
	class UMyViewManager* ViewManager;

};

// 🔥 UE 有两套完全独立的输入系统！
// 1. 游戏世界输入（GameWorld Input）
// PlayerController Tick → PlayerInput → InputComponent
// 跑在游戏线程，按帧执行（Tick）
// 2. UI 输入（Slate Input）
// SlateApplication → InputPreProcessors → Widget
// 操作系统消息驱动，事件触发（Event Driven）
