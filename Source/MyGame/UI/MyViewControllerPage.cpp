// Fill out your copyright notice in the Description page of Project Settings.

#include "MyViewControllerPage.h"

void UMyViewControllerPage::Init(UUserWidget* InRootView)
{
	if (LifeState != EUILifeState::None) return;

	RootView = InRootView;
	LifeState = EUILifeState::Initialized;
	OnInit();
}

void UMyViewControllerPage::Show()
{
	if (RootView.IsValid())
	{
		RootView->SetVisibility(ESlateVisibility::Visible);
		LifeState = EUILifeState::Visible;
		OnShow();
	}
}

void UMyViewControllerPage::Hide()
{
	DestroyAllPanels();

	if (RootView.IsValid())
	{
		RootView->SetVisibility(ESlateVisibility::Hidden);
	}

	LifeState = EUILifeState::Hidden;
	OnHide();
}
