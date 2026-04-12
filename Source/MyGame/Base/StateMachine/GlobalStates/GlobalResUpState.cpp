// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalResUpState.h"

#include "Blueprint/UserWidget.h"
#include "StartUp/ResUpdatePage.h"
#include "ChunkDownloader.h"
#include "MyGameInstance.h"
#include "PipelineFileCache.h"

void UGlobalResUpState::BeginState()
{
	Super::BeginState();
}

void UGlobalResUpState::EndState()
{
	Super::EndState();
	DestroyResUpWidget();
}

void UGlobalResUpState::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void UGlobalResUpState::ConfigEnterStateCoroutine(FCoroutine& InCoroutine)
{
	Super::ConfigEnterStateCoroutine(InCoroutine);

	const bool bEnableUpdate = IsEnableUpdate();
	UE_LOG(LogTemp, Log, TEXT("Is Update Res Enable %d"), bEnableUpdate);
	
	if (bEnableUpdate)
	{
		InCoroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&](){
			// 先做GC
			GEngine->ForceGarbageCollection(true);
		}));
		InCoroutine.ConfigNewAction(FCoroutineAction::CreateWeakLambda(this, [&]()
		{
			ConfigUpdateMethod(InCoroutine);
		}));	
	}
	else
	{
		ShowResUpWidget();
		ConfigShaderCompileProcess();//TransferToNextState();
	}
}

void UGlobalResUpState::ConfigUpdateMethod(FCoroutine& InCoroutine)
{
	ShowResUpWidget();
#if !UE_BUILD_SHIPPING
	// PakDownloader逻辑
	UE_LOG(LogTemp, Log, TEXT("PakChunkDown Config Process!"));
	ConfigChunkDownloaderProcess();
#endif
}

void UGlobalResUpState::ConfigChunkDownloaderProcess()
{
	// 初始化
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&]()
	{
		const FString DeploymentName = "PMGame";
		// initialize the chunk downloader
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetOrCreate();
		FString PlatName = "WindowsNoEditor";
		Downloader->Initialize(PlatName, 8);
		// load the cached build ID
		Downloader->LoadCachedBuild(DeploymentName);
		// update the build manifest file
		TFunction<void(bool bSuccess)> UpdateCompleteCallback = [&](bool bSuccess)
		{
			bFinishCbDownloadManifest = true;
			bIsDownloadManifestUpToDate = bSuccess;
		};
		// 构建版本号
		FString ContentBuildId = TEXT("0.0.1");
		Downloader->UpdateBuild(DeploymentName, ContentBuildId, UpdateCompleteCallback);
		if (WidgetInst)
		{
			WidgetInst->SetPhaseString("Initialize...");
		}
	}));
	// 等待Manifest更新完成
	Coroutine.ConfigWaitAction(FCoroutineWaitAction::CreateLambda([&]()
	{
		return bFinishCbDownloadManifest;
	}));
	// 检查结果
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&]()
	{
		UE_LOG(LogTemp, Log, TEXT("ChunkDownloader Manifest load finish!ret=%d"), bIsDownloadManifestUpToDate);
		if (bFinishCbDownloadManifest && !bIsDownloadManifestUpToDate )
		{
			UE_LOG(LogTemp, Log, TEXT("Can't load manifest"));
			// 销毁
			TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
			Downloader->Finalize();
			ConfigErrStatusAction();
			return;
		}
		// MountPak接口（修复：严格匹配UE4委托签名）
		if (!FCoreDelegates::MountPak.IsBound())
		{
			FCoreDelegates::MountPak.BindLambda([](const FString& Path, int32 PakOrder) -> IPakFile*
			{
				UE_LOG(LogTemp, Log, TEXT("Mount Pak: %s"), *Path);
				// 必须返回指针，不能return bool！
				return nullptr;
			});
		}
	}));
	// 更新进度并等待Mount完成
	Coroutine.ConfigWaitAction(FCoroutineWaitAction::CreateLambda([&]()
	{
		return bIsDownloadManifestUpToDate;
	}));
	// 下载chunk
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&]()
	{
		TArray<int32> ChunkDownloadList;
		// get the chunk downloader
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
		Downloader->GetAllChunkIds(ChunkDownloadList);
		TFunction<void (bool bSuccess)> DownloadCompleteCallback = [&, this](bool bSuccess)
		{
			if (this) OnDownloadComplete(bSuccess);
		};
		Downloader->DownloadChunks(ChunkDownloadList, DownloadCompleteCallback, 1);

		// start loading mode
		TFunction<void (bool bSuccess)> LoadingModeCompleteCallback = [&, this](bool bSuccess)
		{
			if (this) OnLoadingModeComplete(bSuccess);
		};
		Downloader->BeginLoadingMode(LoadingModeCompleteCallback);
		if (WidgetInst)
		{
			WidgetInst->SetPhaseString("Downloading...");
		}
	}));

	// 等待下载完成
	Coroutine.ConfigWaitAction(FCoroutineWaitAction::CreateLambda([&]()
	{
		// 更新进度
		UpdateChunkDownloaderProgress();
		return bIsDoneLoadingMode;
	}));
	// Mount
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&]()
	{
		TArray<int32> ChunkDownloadList;
		// get the chunk downloader
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
		Downloader->GetAllChunkIds(ChunkDownloadList);
		TFunction<void (bool bSuccess)> MountCompleteCallback = [&, this](bool bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("MountCallback Ret = %d"), bSuccess);
			bMountTrunksComplete = true;
		};
		Downloader->MountChunks(ChunkDownloadList, MountCompleteCallback);
		if (WidgetInst)
		{
			WidgetInst->SetProgress(1.0f);
		}
	}));
	Coroutine.ConfigWaitAction(FCoroutineWaitAction::CreateLambda([&]()
	{
		return bMountTrunksComplete;
	}));
	// 到登录界面
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&]()
	{
		ConfigShaderCompileProcess(); //TransferToNextState();
	}));
}

void UGlobalResUpState::ConfigShaderCompileProcess()
{
	UE_LOG(LogTemp, Log, TEXT("UPMGlobalResUpState::ConfigShaderCompileProcess"));
	
	bool BeginCompile = false;
	if(!BeginCompile)
	{

		// Mount
		Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&]()
		{
			// 设置4秒超时保护：无论视频是否播完，4秒后强制进入下一步
			// 获取世界计时器管理器

			CurrentPercent =0;
			if (WidgetInst)
			{
				WidgetInst->SetPhaseString("Downloading...");
				WidgetInst->SetProgress(CurrentPercent);
			}
			auto& TimerManager = GetWorld()->GetTimerManager();
			TimerManager.SetTimer(SimulateTimerHandle, FTimerDelegate::CreateUObject(this, &UGlobalResUpState::SimulateRefreshPercent), 0.2, true);
		}));

		Coroutine.ConfigWaitAction(FCoroutineWaitAction::CreateLambda([&]()
		{
			return CurrentPercent >= 1;
		}));

		
		Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&]()
		{
			TransferToNextState();
		}));
	}
}

void UGlobalResUpState::SimulateRefreshPercent()
{
	CurrentPercent= CurrentPercent+0.1;
	if (WidgetInst)
	{
		WidgetInst->SetProgress(CurrentPercent);
	}
	if(CurrentPercent >= 1)
	{
		ClearSimulateTimer();
	}
}

void UGlobalResUpState::UpdateChunkDownloaderProgress()
{
}

void UGlobalResUpState::ShowResUpWidget()
{
	// 新加载一个UI
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return;
	}
	UClass* WidgetClass = WidgetClassResUpdate.LoadSynchronous();
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Load WidgetClassResUpdate failed:%s"), *WidgetClassResUpdate.ToString());
		return;
	}
	
	WidgetInst = Cast<UResUpdatePage>(CreateWidget(World, WidgetClass));
	if (WidgetInst != nullptr)
	{
		WidgetInst->AddToViewport();
	}
}

void UGlobalResUpState::DestroyResUpWidget()
{
	if (WidgetInst)
	{
		WidgetInst->RemoveFromViewport();
	}
}

void UGlobalResUpState::ClearSimulateTimer()
{
	// 判断计时器句柄是否有效
	if (SimulateTimerHandle.IsValid())
	{
		// 安全获取世界，清除正在运行的计时器
		if (auto World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(SimulateTimerHandle);
		}

		// 将计时器句柄标记为无效，确保IsValid()返回false，避免重复清理
		SimulateTimerHandle.Invalidate();
	}
}

void UGlobalResUpState::OnDownloadComplete(bool bSuc)
{
	UE_LOG(LogTemp, Log, TEXT("Downloadpak complete!ret=%d"), bSuc);
}

void UGlobalResUpState::OnLoadingModeComplete(bool bSuc)
{
	UE_LOG(LogTemp, Log, TEXT("Downloadpak complete!ret=%d"), bSuc);
}

void UGlobalResUpState::TransferToNextState()
{
	UGlobalInitStateEventData* Data = NewObject<UGlobalInitStateEventData>();
	Data->DebugMessage = TEXT("ResUpdate Finish");
	if (const UGlobalResUpStateEventData* TrigData = Cast<const UGlobalResUpStateEventData>(GetTriggerEventData()))
	{
		if (TrigData->ResUpReason == EResUpReasonType::FirstResourceUpdate)
		{
			UE_LOG(LogTemp, Log, TEXT("Transfer from resup to init"));
			Data->bInitForStartup = true;
			Data->DebugMessage = TEXT("StartupResUpdate Finish");
			Data->PtrGameStartPageInst = TrigData->PtrGameStartPageInst;
		}
	}

	TransferState(EGlobalStateType::Init, Data);
}

void UGlobalResUpState::ConfigErrStatusAction()
{
	Coroutine.Reset();
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&]()
	{
		if (WidgetInst)
		{
			WidgetInst->SetPhaseString(TEXT("Failed on download resource!"));
		}
	}));
	Coroutine.ConfigWaitForSeconds(1.0f);
	Coroutine.ConfigNewAction(FCoroutineAction::CreateLambda([&]()
	{
		ConfigShaderCompileProcess(); //TransferToNextState();
	}));
	Coroutine.Run();
}

bool UGlobalResUpState::IsEnableUpdate()
{
	return false;
}