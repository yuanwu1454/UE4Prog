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
	int LuaOpenDataRef=0;
};


struct FMyUITableRow;
class UMyUserWidget;
/**
 * 
 */
UCLASS(config=Game)
class MYGAME_API UMyViewManager : public UMyGameInstanceSubsystem
{
	GENERATED_BODY()
	virtual void OnStartUp() override;
	virtual void OnShutDown() override;
public:
	UPROPERTY()
	TArray<UMyViewControllerPage*> AllPageList; // 存放所有Page指针,直到退出游戏

	UFUNCTION(BlueprintCallable)
	static UMyViewManager* Get(const UObject* ContextObject);
	// 打开界面
	UMyViewControllerPage* OpenPage(const FName& UIName, bool bNewInstance = false, UPageOpenData* OpenData = nullptr);
	template<class T>
	T* OpenPage(const FName& UIName, bool bNewInstance = false, UPageOpenData* OpenData = nullptr)
	{
		return Cast<T>(OpenPage(UIName, bNewInstance, OpenData));
	}
	void GetUITableRowWithUIName(const FName& UIName, const FMyUITableRow*& Info) const;
	UMyViewControllerPage* FindPage(const FName& UIName) const;
	
    template<class T>
    T* FindPage(const FName& UIName)
    {
    	return Cast<T>(FindPage(UIName));
    }
	// 关闭界面 唯一页面
	bool ClosePage(const FName& UIName);
	bool HidePage(const FName& UIName);
	bool ShowPage(const FName& UIName);
	bool ProcessKeyEvent(FKey Key, EInputEvent InputEvent);

	void CloseAllPage();
};
