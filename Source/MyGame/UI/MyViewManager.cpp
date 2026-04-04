// Fill out your copyright notice in the Description page of Project Settings.


#include "MyViewManager.h"

#include "BaseUserWidget.h"
#include "MyUITableRow.h"
#include "MyUserWidget.h"
#include "SlateEventsHelper.h"
#include "Engine/Console.h"
#include "TableManager/MyBasicTableManager.h"


void UMyViewManager::OnStartUp()
{
	Super::OnStartUp();

	if(auto MyInputProcessor =  FSlateEventsHelper::Get().GetMyInputProcessor())
	{
		MyInputProcessor->SetViewManager(this);
	}
}

void UMyViewManager::OnShutDown()
{
	Super::OnShutDown();
}

UMyViewManager* UMyViewManager::Get(const UObject* ContextObject)
{
	if (ContextObject)
	{
		UWorld* World = ContextObject->GetWorld();
		if (World && World->GetGameInstance())
		{
			return World->GetGameInstance()->GetSubsystem<UMyViewManager>();
		}
	}
	return nullptr;
}

bool UMyViewManager::ClosePage(const FName& UIName)
{
	UMyViewControllerPage* Page = FindPage(UIName);
	UE_LOG(LogTemp, Log, TEXT("UPMViewManager::ClosePage %s,exist :%d"), *(UIName.ToString()), !!Page);
	if (Page)
	{
		// HideOrClosePageView(Page, true);
		bool bRet = false;
		if(Page->RootView.IsValid())
		{
			Page->RootView.Get()->RemoveFromViewport();
			bRet = true;
		}
		AllPageLst.Remove(Page);
		return bRet;
	}
	return false;
}

bool UMyViewManager::HidePage(const FName& UIName)
{
	UMyViewControllerPage* Page = FindPage(UIName);
	UE_LOG(LogTemp, Log, TEXT("UPMViewManager::ClosePage %s,exist :%d"), *(UIName.ToString()), !!Page);
	if (Page)
	{
		if(Page->RootView.IsValid())
		{
			Page->RootView.Get()->SetVisibility(ESlateVisibility::Collapsed);
			return true;
		}
	}
	return false;
}

bool UMyViewManager::ShowPage(const FName& UIName)
{
	UMyViewControllerPage* Page = FindPage(UIName);
	UE_LOG(LogTemp, Log, TEXT("UPMViewManager::ClosePage %s,exist :%d"), *(UIName.ToString()), !!Page);
	if (Page)
	{
		if(Page->RootView.IsValid())
		{
			Page->RootView.Get()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			return true;
		}
	}
	return false;
}

bool UMyViewManager::ProcessKeyEvent(FKey Key, EInputEvent InputEvent)
{
#if !UE_BUILD_SHIPPING
	//ugly code seeing if the console is open
	if (GetGameInstance())
	{
		if (UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient())
		{
			UConsole* ViewportConsole = GameViewportClient->ViewportConsole;
			if ( ViewportConsole != nullptr &&  ViewportConsole->ConsoleActive() ) 
			{
				return false;
			}
		}
	}
#endif

	for (auto P : AllPageLst)
	{
		if(P->RootView.IsValid() && P->RootView->IsInViewport()&& P->RootView->IsVisible())
		{
			UBaseUserWidget* Widget = Cast<UBaseUserWidget>(P->RootView);
			if(Widget->HandleKeyEvent(Key, InputEvent))
			{
				UE_LOG(LogTemp, Log, TEXT("UPMViewManager::ProcessKeyEvent %s Key:%s Event:%d"), *Widget->GetName(), *Key.GetFName().ToString(), InputEvent);
				return true;
			}
		}
	}
	return false;
}

UMyViewControllerPage* UMyViewManager::OpenPage(const FName& UIName, bool bNewInstance, UPageOpenData* OpenData)
{
	if (UIName == NAME_None)
	{
		return nullptr;
	}

	const FMyUITableRow* Info = nullptr;
	GetUITableRowWithUIName(UIName, Info);
	if (Info == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UPMViewManager::OpenPage UIPageInfo Cannot Find %s"), *(UIName.ToString()));
		return nullptr;
	}
	
	// 保护TearingDown前移
    UWorld* World = GetWorld();
    if (World)
    {
    	// 标记当前世界是否正在销毁 / 关闭
		// 当 Level 切换
		// 当 GameInstance 切换
		// 当 游戏退出
		// 当 PIE 结束
		// 引擎会把它设为 true
    	if(World->bIsTearingDown)
    	{
    		UE_LOG(LogTemp, Warning, TEXT("UPMViewManager::OpenPage Widget UIName : %s - Attempting to be created while tearing down the world '%s'"),*(UIName.ToString()) , *(World->GetName()));
    		return nullptr;	
    	}
    	
    	if(!World->IsValidLowLevel())
    	{
    		UE_LOG(LogTemp, Warning, TEXT("UPMViewManager::OpenPage Widget UIName : %s - Attempting to be created while Not ValidLowLevel World"),*(UIName.ToString()));
    		return nullptr;	
    	}
    }

	// 已经存在 直接就展示
	if (auto ControllerPage = FindPage(UIName))
	{
		if(ShowPage(UIName))
		{
			return ControllerPage;
		}
	}

	TSubclassOf<UUserWidget> ViewClass = LoadClass<UUserWidget>(nullptr, *Info->ViewClass.GetAssetPathName().ToString());
	UObject* OwningObject = nullptr;
	UUserWidget* View = CreateWidget(World, ViewClass);

	UE_LOG(LogTemp, Log, TEXT("PMViewManager::OpenPage CreateWidget ViewClass: %s, OwningObject: %s"), *(UIName.ToString()), *GetNameSafe(OwningObject));
	if (View == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("PMViewManager::OpenPage ViewClass Cannot Find %s"), *(UIName.ToString()));
		return nullptr;
	}
	
	UMyViewControllerPage* ViewController =NewObject<UMyViewControllerPage>(this);
	if (!ViewController)
	{
		UE_LOG(LogTemp, Error, TEXT("PMViewManager::OpenPage Controller Class Cannot Find %s"), *(UIName.ToString()));
		return nullptr;
	}
	View->AddToViewport(Info->Order);
	
	if (ViewClass && ViewClass->IsChildOf(UBaseUserWidget::StaticClass()))
	{
		if(auto BaseUserWidget = Cast<UBaseUserWidget>(View))
		{
			BaseUserWidget->UIName = UIName;
		}
	}
	ViewController->PageName = UIName;
	ViewController->RootView = View;
	AllPageLst.Add(ViewController);
	return ViewController;
}

void UMyViewManager::GetUITableRowWithUIName(const FName& UIName, const FMyUITableRow*& Info) const
{
	Info = UMyBasicTableManager::GetUITable<FMyUITableRow>(UIName);
}
UMyViewControllerPage* UMyViewManager::FindPage(const FName& UIName) const
{
	// Todo: Find all pages if needed.
	for (UMyViewControllerPage* Page : AllPageLst) 
	{
		if (Page && !IsValid(Page))
		{
			UE_LOG(LogTemp, Error, TEXT("PMViewManager::FindPage InValid Page When find %s"), *(UIName.ToString()));
		}
		// Assume each page will be created only once so the first found page is what we want
		if (Page && Page->PageName == UIName)
		{
			return Page;
		}
	}
	return nullptr;

}