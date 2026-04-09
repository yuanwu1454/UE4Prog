// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUserWidget.h"
#include "ResUpdatePage.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UResUpdatePage : public UBaseUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;

	// 设置进度
	void SetProgress(float ProgressValue);

	// 获取进度
	float GetProgress();

	// 设置下载进度
	void SetDownladProgressText(FString ProgText);

	// 设置更新阶段
	void SetPhaseString(FString Str);
protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	// 进度
	UPROPERTY(BlueprintReadOnly, Category= Widget, meta=(BindWidget))
	class UProgressBar* ProgressBar;

	// 背景
	UPROPERTY(BlueprintReadOnly, Category= Widget, meta=(BindWidget))
	class UImage* Img_Bg;

	// 进度文字展示
	UPROPERTY(BlueprintReadOnly, Category= Widget, meta=(BindWidget))
	class UTextBlock* Txt_Progress;

	// 进度阶段段展示
	UPROPERTY(BlueprintReadOnly, Category= Widget, meta=(BindWidget))
	class UTextBlock* Txt_Phase;
	
	// 文本提示
	UPROPERTY(EditAnywhere)
	TArray<FString> Tips;

	UPROPERTY(EditAnywhere)
	float CatchUpTime = 0.5f;

	UPROPERTY(Transient)
	float TargetProgressValue = 0.0f;

	UPROPERTY(Transient)
	float CatchUpSpeed = 0.0f;
};
