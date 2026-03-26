// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MyUserWidget.generated.h"

// 测试动态委托（给 IsBindableEvent 用）
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FString, FOnGetCustomText, int32, InIndex);

/**
 * 
 */
UCLASS()
class MYGAME_API UMyUserWidget : public UBaseUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:

	// 1. BindWidget：必须绑定，蓝图里必须有同名控件，否则报错
	UPROPERTY(meta=(BindWidget))
	UButton* Button_A;  // 变量名必须和蓝图中Button的名字完全一致（buttonA）


	
	// -------------------------------------------------------------------------
	// 2. BindWidgetOptional：可选绑定，没有也不报错
	// -------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, meta = (BindWidgetOptional), Category = "Test UMG Meta")
	class UTextBlock* Text_Optional = nullptr;
	
	// -------------------------------------------------------------------------
	// 3. BindWidgetAnim：绑定动画
	// -------------------------------------------------------------------------
	// UPROPERTY(VisibleAnywhere, meta = (BindWidgetAnim), Category = "Test UMG Meta")
	// UWidgetAnimation* Anim_FadeIn = nullptr;

	// UPROPERTY(VisibleAnywhere, meta = (BindWidgetAnimOptional), Category = "Test UMG Meta")
	// UWidgetAnimation* Anim_Loop = nullptr;
	
	// -------------------------------------------------------------------------
	// 4. DesignerRebuild：改这个值，设计器立刻刷新预览
	// -------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Test UMG Meta", meta = (DesignerRebuild))
	bool bRefreshDesignerWhenChange = false;

	// -------------------------------------------------------------------------
	// 5. IsBindableEvent：在 Details 面板显示“绑定”按钮
	// -------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Test UMG Meta", meta = (IsBindableEvent))
	FOnGetCustomText OnGetCustomText;


private:// 3. 按钮点击事件的回调函数（蓝图也可调用，方便扩展）
    UFUNCTION()  // 必须加UFUNCTION()，否则绑定会失败
    void OnButtonAClicked();


	void PrintAllWidgetVariableNames();
	FString GetWidgetVariableName(UWidget* Widget);
};
