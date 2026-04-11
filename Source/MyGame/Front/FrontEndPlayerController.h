// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/PlayerControllerBase.h"
#include "FrontEndPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API AFrontEndPlayerController : public APlayerControllerBase
{
	GENERATED_BODY()

	AFrontEndPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override {}
	UFUNCTION(BlueprintCallable)
	virtual void SetupInputComponent() override;
	virtual void NotifyLoadedWorld(FName WorldPackageName, bool bFinalDest) override;

	virtual void BuildInputStack(TArray<UInputComponent *>& InputStack) override;
	
	//统一开关输入响应
	bool bEnableInput = true;
	void SetEnableInput(bool bEnable);

	//3d ui 交互逻辑实现
	#pragma region 3DUIInteraction

	//设置是否开启3D UI交互
	void SetEnable3DUIInteraction(bool bEnable);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	class UMyWidgetInteractionComponent* MyWidgetInteractionComp;

#pragma endregion 3DUIInteraction
};
