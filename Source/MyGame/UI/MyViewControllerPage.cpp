// Fill out your copyright notice in the Description page of Project Settings.

#include "MyViewControllerPage.h"

// void UMyViewControllerPage::Init(UUserWidget* InRootView)
// {
// 	if (LifeState != EUILifeState::None) return;
//
// 	RootView = InRootView;
// 	LifeState = EUILifeState::Initialized;
// }
//
// void UMyViewControllerPage::Show()
// {
// 	if (RootView.IsValid())
// 	{
// 		RootView->SetVisibility(ESlateVisibility::Visible);
// 		LifeState = EUILifeState::Visible;
// 		OnShow();
// 	}
// }
//
// void UMyViewControllerPage::Close()
// {
// 	CloseAllPanels();
//
// 	if (RootView.IsValid())
// 	{
// 		RootView->SetVisibility(ESlateVisibility::Hidden);
// 	}
//
// 	LifeState = EUILifeState::Close;
// 	OnClose();
// }
//
//
// void UMyViewControllerPage::Release()
// {
// 	CloseAllPanels();
//
// 	if (RootView.IsValid())
// 	{
// 		RootView->SetVisibility(ESlateVisibility::Hidden);
// 	}
//
// 	LifeState = EUILifeState::Destroyed;
// 	OnRelease();
// }
