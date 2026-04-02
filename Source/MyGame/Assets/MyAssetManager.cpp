// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAssetManager.h"

UMyAssetManager::UMyAssetManager()
{
}

UMyAssetManager::UMyAssetManager(const FObjectInitializer& ObjectInitializer)
{
}

UMyAssetManager& UMyAssetManager::Get()
{
	UMyAssetManager* MyAssetManager = Cast<UMyAssetManager>(&UAssetManager::Get());
	return *MyAssetManager;
}


TSharedPtr<FStreamableHandle> UMyAssetManager::RequestSyncLoad(FSoftObjectPath& TargetToStream,
                                                               bool bManageActiveHandle, FString DebugName)
{
	TSharedPtr<FStreamableHandle> AssetHandle = StreamableManager.RequestSyncLoad(TargetToStream, bManageActiveHandle, DebugName);
	return AssetHandle;
}
