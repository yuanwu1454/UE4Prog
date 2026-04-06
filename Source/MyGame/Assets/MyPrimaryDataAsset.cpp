// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPrimaryDataAsset.h"

UMyPrimaryDataAsset::UMyPrimaryDataAsset()
{
}

UMyPrimaryDataAsset::UMyPrimaryDataAsset(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

FPrimaryAssetId UMyPrimaryDataAsset::GetPrimaryAssetId() const
{
	return Super::GetPrimaryAssetId();
}

void UMyPrimaryDataAsset::PostLoad()
{
	Super::PostLoad();
}

void UMyPrimaryDataAsset::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();
}

void UMyPrimaryDataAsset::PreSave(const ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);
}
