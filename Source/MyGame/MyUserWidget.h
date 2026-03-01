// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	// ========== 辅助函数 ==========
	/**
	 * 切换到UI输入模式（游戏+UI 或 仅UI）
	 */
	void SwitchToUIMode();

	/**
	 * 恢复到游戏输入模式
	 */
	void RestoreToGameMode();

	/**
	 * 安全获取当前玩家控制器
	 * 避免空指针报错
	 */
	APlayerController* GetSafePlayerController();
	
	UPROPERTY(meta=(BindWidget))
	UButton* Button_A;  // 变量名必须和蓝图中Button的名字完全一致（buttonA）
	
	UFUNCTION(BlueprintCallable)
	UWorld* GetSelfWorld();

private:// 3. 按钮点击事件的回调函数（蓝图也可调用，方便扩展）
    UFUNCTION()  // 必须加UFUNCTION()，否则绑定会失败
    void OnButtonAClicked();


	void PrintAllWidgetVariableNames();
	FString GetWidgetVariableName(UWidget* Widget);
	void PrintAllProp();
	FString GetPropertyTypeName(FProperty* Prop);
	// 辅助函数：解析EFunctionFlags为可读字符串（UE4.27专用）
	FString GetFunctionFlagsString(EFunctionFlags Flags);
	FString GetBlueprintFunctionReturnType(UFunction* Func);
};
