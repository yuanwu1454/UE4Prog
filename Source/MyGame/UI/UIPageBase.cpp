// Fill out your copyright notice in the Description page of Project Settings.

#include "UIPageBase.h"

void UUIPageBase::Init(UUserWidget* InRootView)
{
	if (LifeState != EUILifeState::None) return;

	RootView = InRootView;
	LifeState = EUILifeState::Initialized;
	OnInit();
}

void UUIPageBase::Open()
{
	if (RootView.IsValid())
	{
		RootView->SetVisibility(ESlateVisibility::Visible);
		LifeState = EUILifeState::Visible;
		OnOpen();
	}
}

void UUIPageBase::Close()
{
	DestroyAllPanels();

	if (RootView.IsValid())
	{
		RootView->SetVisibility(ESlateVisibility::Hidden);
	}

	LifeState = EUILifeState::Hidden;
	OnClose();
}

void UUIPageBase::DestroyPanel(UUIPanelBase* Panel)
{
	if (!Panel) return;
	Panel->Release();
	Panels.Remove(Panel);
}

void UUIPageBase::DestroyAllPanels()
{
	for (auto& WeakPanel : Panels)
	{
		if (WeakPanel.IsValid())
			WeakPanel->Release();
	}
	Panels.Empty();
}