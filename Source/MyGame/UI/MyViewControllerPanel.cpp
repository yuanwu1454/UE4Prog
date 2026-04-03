// Fill out your copyright notice in the Description page of Project Settings.

#include "MyViewControllerPanel.h"

#include "MyViewControllerPage.h"

// void UMyViewControllerPanel::Init(UUserWidget* InView, UMyViewController* InOwnerPage)
// {
// 	if (LifeState != EUILifeState::None) return;
//
// 	RootView = InView;
// 	OwnerPage =  InOwnerPage;
// 	
// 	LifeState = EUILifeState::Initialized;
// 	OnInit();
// }
//
// void UMyViewControllerPanel::Close()
// {
// 	if (LifeState == EUILifeState::Close) return;
//
// 	Hide();
// 	OnRelease();
// 	LifeState = EUILifeState::Close;
// 	RootView.Reset();
// 	OwnerPage.Reset();
// }
//
// void UMyViewControllerPanel::Open()
// {
// 	if (RootView.IsValid())
// 	{
// 		RootView->SetVisibility(ESlateVisibility::Visible);
// 		LifeState = EUILifeState::Visible;
// 		OnShow();
// 	}
// }
//
// void UMyViewControllerPanel::Close()
// {
// 	if (RootView.IsValid())
// 	{
// 		RootView->SetVisibility(ESlateVisibility::Hidden);
// 		LifeState = EUILifeState::Hidden;
// 		OnHide();
// 	}
// }
