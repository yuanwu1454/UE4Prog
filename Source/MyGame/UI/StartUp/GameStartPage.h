// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUserWidget.h"
#include "GameStartPage.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UGameStartPage : public UBaseUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;

	void StartPresentation();
	
	UFUNCTION()
	void AfterPlayTXLogo();
	
	void CyLogoFadeIn();

	UFUNCTION()
	void StartFlowEnd();
	
	void WillClosePage();

protected:
	void ClearOutTimeTimer();

public:
	FWidgetAnimationDynamicEvent AnimEndEvent;
	
protected:
	UPROPERTY(EditAnywhere)
	class UMediaPlaylist* LogoVideoPlaylist;

	UPROPERTY(EditAnywhere)
	class UFileMediaSource* LogoVideoFile;

	UPROPERTY(EditAnywhere)
	class UFileMediaSource* LogoFile;
	
	UPROPERTY(EditAnywhere)
	class UMediaPlayer* VideoMediaPlayer;

	UPROPERTY(meta=(BindWidget))
	UImage* Logo;

	UPROPERTY(meta=(BindWidget))
	UImage* ImgVideoBg;

	UPROPERTY(BlueprintReadOnly, Category = Widget, meta = (BindWidgetAnimOptional))
	class UWidgetAnimation* AnimLogoFadeIn;

	UPROPERTY()
	FTimerHandle OutTimeTimerHandle;

	UPROPERTY(meta=(BindWidgetOptional))
	class UWidgetSwitcher* Switcher_Logo;
};
