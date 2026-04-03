// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UICommon.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "MyViewController.generated.h"


class UMyViewControllerPanel;
/**
 * 简单的viewcontroller 暂时只是持有uuserwidget
 */
UCLASS(Abstract)
class MYGAME_API UMyViewController : public UObject
{
	GENERATED_BODY()
// protected:
// 	virtual void OnOpen(){}
// 	virtual void OnClose(){}	
// public:
	// virtual void Init(UUserWidget* InRootView);
// 	virtual void Open(){};
// 	virtual void Close(){};
public:
	//UObject interface
	virtual class UWorld* GetWorld() const override;

	/**
	 * Gets the player controller associated with this UI.
	 * @return The player controller that owns the UI.
	 */
	// virtual APlayerController* GetOwningPlayer() const;

	// void SetCacheView(UUserWidget* InView);
	// UUserWidget* GetCacheView() const;
	//
	// template<class T>
	// T* GetCacheView()
	// {
	// 	return Cast<T>(GetCacheView());
	// }

	// template<typename T>
	// T* CreatePanel(TSubclassOf<UMyViewController> PanelClass)
	// {
	// 	if (!PanelClass) return nullptr;
	// 	T* Panel = NewObject<T>(this, PanelClass);
	// 	Panels.Add(Panel);
	// 	return Panel;
	// }

	// void ClosePanel(UMyViewControllerPanel* Panel);
	// void CloseAllPanels();

	// EUILifeState GetLifeState() const { return LifeState; }

	// TArray<TWeakObjectPtr<UMyViewController>> Panels;

	// EUILifeState LifeState = EUILifeState::None;
public:
	FName PageName = NAME_None;
	TWeakObjectPtr<UUserWidget> RootView;
};
