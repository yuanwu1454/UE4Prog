// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyViewController.h"
#include "UnLuaInterface.h"
#include "Blueprint/UserWidget.h"
#include "BaseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UBaseUserWidget : public UUserWidget, public IUnLuaInterface
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FString GetModuleName_Implementation() const override;



public:
	FName UIName;
	
protected:
	// ========== 通用UI辅助函数 ==========

	/**
	 * 恢复到游戏输入模式
	 */
	void RestoreToGameMode();

	/**
	 * 安全获取当前玩家控制器
	 * 避免空指针报错
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Base")
	APlayerController* GetSafePlayerController();

	/**
	 * 关闭当前UI
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Base")
	void CloseUI();

	/**
	 * 获取自身所在的World
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Base")
	UWorld* GetSelfWorld();

	// Lua文件路径，不需要扩展名
	UPROPERTY(EditDefaultsOnly, AssetRegistrySearchable, Category = "Config | Lua")
	FString LuaModuleName;

	UFUNCTION(BlueprintImplementableEvent)
	void OnLuaOpen();

	UFUNCTION(BlueprintImplementableEvent)
	void OnLuaReactiveFocus();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnLuaShow();

	UFUNCTION(BlueprintImplementableEvent)
	void OnLuaHide();

	UFUNCTION(BlueprintImplementableEvent)
	void OnLuaClose();

};
