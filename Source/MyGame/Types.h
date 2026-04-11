// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types.generated.h"

UENUM(BlueprintType)
enum class EGamePlayLoadingPhase: uint8
{
	Connectiing, // 连接中，进度0~10
	LoadingMap,  // 加载地图ing, 进度10~100
	SyncBeginPlay, // 等待开局
};

USTRUCT(BlueprintType)
struct FGamePlayLoadingStateParams
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	EGamePlayLoadingPhase LoadingPhase; // 当前的加载阶段
	UPROPERTY()
	float Percent; // 进度是Range(0,100)
};


//FBuiltinEditorModes
/** The shorthand identifier used for UI Name */
typedef FName FUINameID;

// UI Name constants
namespace FUINames
{
	const FUINameID EM_LoadingAssetPage(TEXT("LoadingAssetPage"));
	const FUINameID EM_LoginPage(TEXT("LoginPage"));
};
