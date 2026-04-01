// Fill out your copyright notice in the Description page of Project Settings.


#include "MyViewController.h"

#include "MyViewControllerPanel.h"

UWorld* UMyViewController::GetWorld() const
{
	if (GetCacheView())
	{
		return GetCacheView()->GetWorld();
	}

	// Could be a GameInstance, could be World, could also be a WidgetTree, so we're just going to follow
	// the outer chain to find the world we're in.
	UObject* Outer = GetOuter();

	while (Outer)
	{
		UWorld* World = Outer->GetWorld();
		if (World)
		{
			return World;
		}

		Outer = Outer->GetOuter();
	}

	return nullptr;
}

APlayerController* UMyViewController::GetOwningPlayer() const
{
	return GetCacheView() ? GetCacheView()->GetOwningPlayer() : nullptr;
}

void UMyViewController::SetCacheView(UUserWidget* InView)
{
	checkf(InView, TEXT("ViewControlle::SetCacheView() View Is Null !"));

	RootView = InView;
}

UUserWidget* UMyViewController::GetCacheView() const
{
	if(RootView.IsValid())
	{
		return RootView.Get();
	}
	return nullptr;
}

void UMyViewController::DestroyPanel(UMyViewControllerPanel* Panel)
{
	if (!Panel) return;
	Panel->Release();
	Panels.Remove(Panel);
}

void UMyViewController::DestroyAllPanels()
{
	for (auto& WeakPanel : Panels)
	{
		if (WeakPanel.IsValid())
			WeakPanel->Release();
	}
	Panels.Empty();
}
