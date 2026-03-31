// Fill out your copyright notice in the Description page of Project Settings.

#include "UIPanelBase.h"

#include "UIPageBase.h"

void UUIPanelBase::Init(UUserWidget* InView, UUIPageBase* InOwnerPage)
{
	if (LifeState != EUILifeState::None) return;

	View = InView;
	OwnerPage =  InOwnerPage;
	
	LifeState = EUILifeState::Initialized;
	OnInit();
}

void UUIPanelBase::Release()
{
	if (LifeState == EUILifeState::Destroyed) return;

	Hide();
	OnRelease();
	LifeState = EUILifeState::Destroyed;
	View.Reset();
	OwnerPage.Reset();
}

void UUIPanelBase::Show()
{
	if (View.IsValid())
	{
		View->SetVisibility(ESlateVisibility::Visible);
		LifeState = EUILifeState::Visible;
		OnShow();
	}
}

void UUIPanelBase::Hide()
{
	if (View.IsValid())
	{
		View->SetVisibility(ESlateVisibility::Hidden);
		LifeState = EUILifeState::Hidden;
		OnHide();
	}
}
