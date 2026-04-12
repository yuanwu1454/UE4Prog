// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/GlobalStateMachine.h"
#include "GlobalResUpState.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MYGAME_API UGlobalResUpState : public UGlobalState
{
	GENERATED_BODY()
	public:
	virtual void BeginState() override;
	virtual void EndState() override;
	virtual void Tick(const float DeltaTime) override;
	
	virtual void ConfigEnterStateCoroutine(FCoroutine& PMCoroutine) override;

private:
	// 配置Dolphin或ChunkDownloader更新
	void ConfigUpdateMethod(FCoroutine& PMCoroutine);
	
	// 配置基于ThunkDownloader的更新
	void ConfigChunkDownloaderProcess();

	void ConfigShaderCompileProcess();

	void SimulateRefreshPercent();
	
	// 下载进度
	void UpdateChunkDownloaderProgress();
	
	// 展示Widget
	void ShowResUpWidget();
	
	// 销毁Widget
	void DestroyResUpWidget();
	

	bool bUseSDKUpdateApk = false;
	bool bSDKUpdateComplete = false;
	
	bool bFinishCbDownloadManifest = false;
	bool bIsDownloadManifestUpToDate = false;
	// 是否可以结束LoadingMode
	bool bIsDoneLoadingMode = false;
	bool bMountTrunksComplete = false;

	bool bSimulateLoadingComplete = false;
	float CurrentPercent = 0;
	FTimerHandle SimulateTimerHandle;

	void ClearSimulateTimer();
	
	uint32 NumPrecompileShaders = 0;

	void OnDownloadComplete(bool bSuc);
	void OnLoadingModeComplete(bool bSuc);

	// Transfer Next State
	void TransferToNextState();

	// 出错情况配置
	void ConfigErrStatusAction();

	// 是否走更新下载流程
	bool IsEnableUpdate();

	// 更新Widget
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<class UResUpdatePage> WidgetClassResUpdate;

	UPROPERTY(Transient)
	class UResUpdatePage* WidgetInst;

	static bool bHasDownloadedAnyRes;
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UDataTable> ResPakTablePath;
	
	int64 FirstResDownBeginTime = 0;
	int64 FirstResProgress = 0;
	int64 FirstResTotalSize = 0;
	bool hasConfirmFirstResUp = false;

public:

	UPROPERTY(EditAnywhere)
	bool bSkip=false;
};
