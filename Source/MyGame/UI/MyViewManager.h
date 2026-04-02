// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubSystem/MyGameInstanceSubsystem.h"
#include "MyViewControllerPage.h"
#include "MyViewManager.generated.h"


UCLASS()
class UPageOpenData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName PageTag;

	// UPROPERTY()
	TMap<FName,void*> DataMap;

	UPROPERTY()
	int LuaOpenDataRef;
};


struct FMyUITableRow;
/**
 * 
 */
UCLASS(config=Game)
class MYGAME_API UMyViewManager : public UMyGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<UMyViewControllerPage*> AllPageLst; // 存放所有Page指针,直到退出游戏


	// 打开界面
	UMyViewControllerPage* OpenPage(const FName& UIName, bool bNewInstance = false, UPageOpenData* OpenData = nullptr);
	template<class T>
	T* OpenPage(const FName& UIName, bool bNewInstance = false, UPageOpenData* OpenData = nullptr)
	{
		return Cast<T>(OpenPage(UIName, bNewInstance, OpenData));
	}
	void GetUITableRowWithUIName(const FName& UIName, const FMyUITableRow*& Info) const;
	bool IsPageOpening(FName Name);
	UMyViewControllerPage* FindPage(const FName& UIName) const;
    
    template<class T>
    T* FindPage(const FName& UIName)
    {
    	return Cast<T>(FindPage(UIName));
    }
};
