// Fill out your copyright notice in the Description page of Project Settings.


#include "MyViewController.h"

#include "MyViewControllerPage.h"
#include "MyViewControllerPanel.h"

// void UMyViewController::Init(UUserWidget* InRootView)
// {
// 	RootView = InRootView;
// }

UWorld* UMyViewController::GetWorld() const
{
	if (RootView.IsValid())
	{
		return RootView.Get()->GetWorld();
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

// APlayerController* UMyViewController::GetOwningPlayer() const
// {
// 	return GetCacheView() ? GetCacheView()->GetOwningPlayer() : nullptr;
// }
//
// void UMyViewController::SetCacheView(UUserWidget* InView)
// {
// 	checkf(InView, TEXT("ViewControlle::SetCacheView() View Is Null !"));
//
// 	RootView = InView;
// }
//
// UUserWidget* UMyViewController::GetCacheView() const
// {
// 	if(RootView.IsValid())
// 	{
// 		return RootView.Get();
// 	}
// 	return nullptr;
// }

// void UMyViewController::ClosePanel(UMyViewControllerPanel* Panel)
// {
// 	if (!Panel) return;
// 	Panel->Release();
// 	Panels.Remove(Panel);
// }
//
// void UMyViewController::CloseAllPanels()
// {
// 	for (auto& WeakPanel : Panels)
// 	{
// 		if (WeakPanel.IsValid())
// 		{
// 			if(UMyViewControllerPanel* Panel = Cast<UMyViewControllerPanel>(WeakPanel.Get()))
// 			{
// 				Panel->Release();				
// 			}else if (UMyViewControllerPage* Page = Cast<UMyViewControllerPage>(WeakPanel.Get()))
// 			{
// 				checkf(false, TEXT("Page is add to Panels!"));
// 			}
// 		}
// 	}
// 	Panels.Empty();
// }
