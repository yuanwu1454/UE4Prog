// Fill out your copyright notice in the Description page of Project Settings.


#include "MyViewManager.h"

#include "MyUITableRow.h"
#include "TableManager/MyBasicTableManager.h"


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


	// 查找已经存在的 Page，找到所有的都打开
	if (!bNewInstance)
	{
		auto Predicator = [Info](UMyViewControllerPage* Obj)
		{
			if (IsValid(Obj))
			{
				return Obj->GetClass() == Info->PageClass && Obj->PageName == Info->PageName;
			}
			return false;
		};

		UMyViewControllerPage** PagePtr = AllPageLst.FindByPredicate(Predicator);

		// if (PagePtr != nullptr && IsValid(*PagePtr))
		// {
		// 	CheckOpenedViewReactiveFocus(*PagePtr);
		// 	if (!IsPageOpening(UIName))
		// 	{
		// 		UINameToPageName.Add(UIName, OpenUIName);
		// 		ShowPageView(*PagePtr);
		// 		(*PagePtr)->Show(OpenData);
		// 	}
		// 	return *PagePtr;
		// }
	}
	
	return nullptr;
}

void UMyViewManager::GetUITableRowWithUIName(const FName& UIName, const FMyUITableRow*& Info) const
{
	Info = UMyBasicTableManager::GetUITable<FMyUITableRow>(UIName);
}

bool UMyViewManager::IsPageOpening(FName Name)
{
	// if (UMyViewControllerPage* Page = FindPage(InName))
	// {
	// 	bool bNeedCheckVisibility = false;
	// 	if(UPMBaseUserWidget* BaseUserWidget = Cast<UPMBaseUserWidget>(Page->GetCacheView()))
	// 	{
	// 		bNeedCheckVisibility = BaseUserWidget->bHideByCollapse;
	// 	}
	// 	if(bNeedCheckVisibility == false)
	// 	{
	// 		return Page->IsOpening();
	// 	}
	// 	else
	// 	{
	// 		if(UPMUserWidget* UserWidget = Cast<UPMUserWidget>(Page->GetCacheView()))
	// 		{
	// 			return Page->IsOpening() && UserWidget->IsVisibleWithPriority(ESlateVisibilityPriority::HideByCollapse);
	// 		}
	// 		else
	// 		{
	// 			return Page->IsOpening() && (Page->GetCacheView() && Page->GetCacheView()->IsVisible());
	// 		}
	// 	}
	// }
	return false;
}

UMyViewControllerPage* UMyViewManager::FindPage(const FName& UIName) const
{
	return nullptr;
}
