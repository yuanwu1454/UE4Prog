// Fill out your copyright notice in the Description page of Project Settings.


#include "SequenceMapManager.h"

#include "GameMapsSettings.h"
#include "MyGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Misc/PackageName.h"
#include "Engine/LevelStreaming.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Logging/LogMacros.h"
#include "MyLogging.h"
#include "TableRow/SequenceMapTableRow.h"

#define LOG_NAMESPACE TEXT("CyUSequenceMapManager")

USequenceMapManager* USequenceMapManager::Get(UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject);
	return GI ? GI->GetSubsystem<USequenceMapManager>() : nullptr;
}

bool USequenceMapManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (Outer && Outer->GetWorld() && Outer->GetWorld()->GetNetMode() == NM_DedicatedServer)
		return false;
	return Super::ShouldCreateSubsystem(Outer);
}

void USequenceMapManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ResetAllData();
	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USequenceMapManager::OnPostLoadMap);
}

void USequenceMapManager::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);
	CancelCurrentLoad();
	ResetAllData();
	Super::Deinitialize();
}

void USequenceMapManager::ResetAllData()
{
	CurrentState = EMapLoadState::None;
	TargetMapId = INDEX_NONE;
	TargetMapPath.Empty();
	TransitionMapName.Empty();
	PendingStreamLevels.Empty();
	CurrentStreamLevelIndex = INDEX_NONE;
	bStreamLevelsLoaded = false;
	bSequenceAssetsLoaded = false;

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void USequenceMapManager::CancelCurrentLoad()
{
	// if (USequenceMapManager::Get(this))
	// {
	// 	USequenceMapManager::Get(this)->OnLevelSequenceLoadCompleted.RemoveAll(this);
	// }

	CurrentState = EMapLoadState::None;
}

bool USequenceMapManager::TeleportMap(int32 InMapId)
{
	// 1. 禁止重复加载
	if (IsMapLoading())
	{
		UE_LOG(Log_Sequence, Warning, TEXT("%s: 已有地图正在加载，拒绝新请求"), LOG_NAMESPACE);
		return false;
	}

	// 2. 获取配置
	// const FCySequenceMapTableRow* MapData = UMyGlobals::Get().GetMapTableRow<FCySequenceMapTableRow>(MapId);

	const FSequenceMapTableRow* MapData = UMyGlobals::Get().GetTableAsset()->GetDataTableRow<FSequenceMapTableRow>(InMapId);
	
	if (!MapData || MapData->MapName.IsEmpty())
	{
		UE_LOG(Log_Sequence, Error, TEXT("%s: 无效地图ID %d"), LOG_NAMESPACE, InMapId);
		return false;
	}

	// 3. 避免重复进入同地图
	if (IsCurrentMap(MapData->MapName))
	{
		UE_LOG(Log_Sequence, Log, TEXT("%s: 当前已在目标地图"), LOG_NAMESPACE);
		return true;
	}
	
	UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		UE_LOG(Log_Sequence, Error, TEXT("%s: World is nullptr"), LOG_NAMESPACE);
		return false;
	}

	FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(World);
	FURL TestURL(&WorldContext.LastURL, *MapData->MapName, TRAVEL_Absolute);
	if (!TestURL.IsLocalInternal())
	{
		UE_LOG(Log_Sequence, Error, TEXT("%s: IsNotLocalInternal"), LOG_NAMESPACE);
		return false;
	}
	
	// 4. 校验地图合法性
	if (!GEngine->MakeSureMapNameIsValid(TestURL.Map))
	{
		UE_LOG(Log_Sequence, Error, TEXT("%s: 地图文件不存在 %s"), LOG_NAMESPACE, *MapData->MapName);
		return false;
	}

	// 5. 启动加载
	StartMapLoad(InMapId, MapData->MapName);
	return true;
}

void USequenceMapManager::StartMapLoad(int32 MapId, const FString& TargetPath)
{
	TargetMapId = MapId;
	TargetMapPath = TargetPath;
	UpdateLoadState(EMapLoadState::Loading);
	const FSequenceMapTableRow* MapData = UMyGlobals::Get().GetTableAsset()->GetDataTableRow<FSequenceMapTableRow>(MapId);
	// 预存流关卡
	if (MapData)
	{
		PendingStreamLevels = MapData->LevelStreamList;
	}

	// 绑定动画加载完成
	// if (auto* SeqMgr = USequenceMapManager::Get(this))
	// {
	// 	bSequenceAssetsLoaded = false;
	// 	SeqMgr->OnLevelSequenceLoadCompleted.AddUniqueDynamic(this, &USequenceMapManager::OnSequenceAssetsLoaded);
	// }
	// else
	// {
	// 	bSequenceAssetsLoaded = true;
	// }

	// 启动过渡关卡
	OpenTransitionLevel();
}

void USequenceMapManager::OpenTransitionLevel()
{
	const UGameMapsSettings* Settings = GetDefault<UGameMapsSettings>();
	if (!Settings || Settings->GetGameDefaultMap().IsEmpty())
	{
		OpenTargetLevelAsync();
		return;
	}

	TransitionMapName = FPackageName::GetShortName(Settings->GetGameDefaultMap());
	const FString OpenCmd = FString::Printf(TEXT("open %s"), *Settings->GetGameDefaultMap());
	UKismetSystemLibrary::ExecuteConsoleCommand(this, OpenCmd);
}

void USequenceMapManager::OpenTargetLevelAsync()
{
	if (TargetMapPath.IsEmpty())
	{
		UpdateLoadState(EMapLoadState::Failed);
		return;
	}

	FLoadPackageAsyncDelegate CompletionDelegate;
	CompletionDelegate.BindUObject(this, &USequenceMapManager::OnAsyncLevelLoadComplete);
	LoadPackageAsync(*TargetMapPath, CompletionDelegate);
}

void USequenceMapManager::OnAsyncLevelLoadComplete(const FName& PackageName, UPackage* Package, EAsyncLoadingResult::Type Result)
{
	if (Result != EAsyncLoadingResult::Succeeded || !Package || CurrentState != EMapLoadState::Loading)
	{
		UpdateLoadState(EMapLoadState::Failed);
		return;
	}

	UWorld* World = UWorld::FindWorldInPackage(Package);
	if (!World)
	{
		UpdateLoadState(EMapLoadState::Failed);
		return;
	}

	OnTargetLevelLoaded(World);
}

void USequenceMapManager::OnTargetLevelLoaded(UWorld* LoadedWorld)
{
	if (!IsCurrentMap(TargetMapPath))
	{
		UGameplayStatics::OpenLevel(this, FName(*TargetMapPath));
	}
}

void USequenceMapManager::OnPostLoadMap(UWorld* World)
{
	if (!IsWorldValid() || !World || CurrentState != EMapLoadState::Loading)
		return;

	// 过渡关完成 → 加载目标关
	if (!TransitionMapName.IsEmpty() && GetShortMapName(World->GetName()) == TransitionMapName)
	{
		OpenTargetLevelAsync();
		return;
	}

	// 目标关加载完成 → 加载流关卡
	if (IsCurrentMap(TargetMapPath))
	{
		if (PendingStreamLevels.Num() > 0)
		{
			StartPreloadStreamLevels();
		}
		else
		{
			bStreamLevelsLoaded = true;
			OnAllResourcesLoaded();
		}
	}
}

bool USequenceMapManager::StartPreloadStreamLevels()
{
	if (PendingStreamLevels.Num() == 0) return false;
	CurrentStreamLevelIndex = 0;
	LoadNextStreamLevel();
	return true;
}

void USequenceMapManager::LoadNextStreamLevel()
{
	if (CurrentStreamLevelIndex >= PendingStreamLevels.Num())
	{
		bStreamLevelsLoaded = true;
		OnAllResourcesLoaded();
		return;
	}

	const FName LevelName = FName(*PendingStreamLevels[CurrentStreamLevelIndex]);
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = TEXT("OnStreamLevelLoaded");
	// 这个linkage 必须需要 不然会出现无法回调
	Info.UUID = 1;
    Info.Linkage = 1;
	
	UGameplayStatics::LoadStreamLevel(this, LevelName, true, true, Info);
}

void USequenceMapManager::OnStreamLevelLoaded()
{
	CurrentStreamLevelIndex++;
	LoadNextStreamLevel();
}

void USequenceMapManager::OnSequenceAssetsLoaded()
{
	bSequenceAssetsLoaded = true;
	// if (auto* SeqMgr = USequenceMapManager::Get(this))
	// {
	// 	SeqMgr->OnLevelSequenceLoadCompleted.RemoveAll(this);
	// }
	OnAllResourcesLoaded();
}

void USequenceMapManager::OnAllResourcesLoaded()
{
	if (bStreamLevelsLoaded && bSequenceAssetsLoaded && CurrentState == EMapLoadState::Loading)
	{
		UpdateLoadState(EMapLoadState::Completed);
	}
}

void USequenceMapManager::UpdateLoadState(EMapLoadState NewState)
{
	if (CurrentState == NewState) return;
	CurrentState = NewState;

	switch (CurrentState)
	{
	case EMapLoadState::Loading:
		OnMapLoadStarted.Broadcast(TargetMapPath);
		break;
	case EMapLoadState::Completed:
		OnMapLoadCompleted.Broadcast(TargetMapId);
		ResetAllData();
		break;
	case EMapLoadState::Failed:
		OnMapLoadFailed.Broadcast(TargetMapId);
		ResetAllData();
		break;
	default: break;
	}
}

// 工具函数
bool USequenceMapManager::IsWorldValid() const
{
	return GetWorld() != nullptr && !GetWorld()->IsPendingKill();
}

FString USequenceMapManager::GetShortMapName(const FString& LongMapPath) const
{
	return FPackageName::GetShortName(LongMapPath);
}

bool USequenceMapManager::IsCurrentMap(const FString& MapPath) const
{
	const FString Current = UGameplayStatics::GetCurrentLevelName(this);
	const FString TargetShort = GetShortMapName(MapPath);
	return Current.Equals(TargetShort, ESearchCase::IgnoreCase);
}