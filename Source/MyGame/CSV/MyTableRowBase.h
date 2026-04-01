// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MyTableRowBase.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct MYGAME_API FMyTableRowBase : public FTableRowBase
{
	GENERATED_BODY()
	FMyTableRowBase();
	virtual ~FMyTableRowBase() override;

	// 导入数据后自动执行逻辑
	// 比如：
	// CSV 里只填了字符串，你要自动转成枚举
	// 自动检查数据合法性（等级不能为负）
	// 自动计算字段（攻击力 = 基础值 * 品质）
	// 导入失败时输出错误日志

	// InDataTable：当前是哪个数据表
	// InRowName：当前这一行的名字（Sword、Potion…）
	// OutCollectedImportProblems：你可以往这里塞错误信息，引擎会在导入时弹窗提示你哪里错了
	// OnPostDataImport	只在 导入 CSV / 重新导入 时调用
	virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override;

	// OnDataTableChanged	编辑器里手动改表格 时调用
	// 在编辑器里手动改 DataTable 时自动调用
	// 比如：
	// 你改了一个数值，另一列自动跟着变
	// 限制数值范围
	// 实时刷新数据
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};



