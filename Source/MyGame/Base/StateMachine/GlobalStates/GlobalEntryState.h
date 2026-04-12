// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/GlobalStateMachine.h"
#include "GlobalEntryState.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MYGAME_API UGlobalEntryState : public UGlobalState
{
	GENERATED_BODY()
public:
	virtual void ConfigEnterStateCoroutine(FCoroutine& PMCoroutine) override;
	void ConfigShowStudioPresent();
	UFUNCTION()
	void OnPresentStudioFinish();
	bool CheckShowStudioPresentFinish() const;
	void TransferToNextState();
private:
	void CreatePresentAnimWidget(UClass* WidgetClass);
	bool bStudioWidgetPresentFinish = false;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> StartupUserWidgetClass;

	UPROPERTY(Transient)
	UUserWidget* GameStartPageInst;
	UPROPERTY(EditAnywhere)
	bool bSkip=false;
};
