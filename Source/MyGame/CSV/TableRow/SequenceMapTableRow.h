#pragma once

#include "CoreMinimal.h"
#include "MyTableRowBase.h"
#include "Engine/DataTable.h"

#include "SequenceMapTableRow.generated.h"
//auto generated start
/**
* 剧情关卡配置表
*/
USTRUCT(BlueprintType)
struct FSequenceMapTableRow : public FMyTableRowBase
{
	GENERATED_USTRUCT_BODY()

	/** 地图ID*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SequenceMap")
	int32 MapId;

	/** 关卡序列所在地图名(配置了会切到对应的地图去播关卡序列，不配则不切地图，在当前地图播)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SequenceMap")
	FString MapName;

	/** 进入地图首先需要加载的关卡（即对应第一场的关卡）*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SequenceMap")
	TArray<FString> LevelStreamList;

	/** 地图名称*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SequenceMap")
	FText NameText;

	/** 地图描述*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SequenceMap")
	FText Desc;

	//auto generated end
};