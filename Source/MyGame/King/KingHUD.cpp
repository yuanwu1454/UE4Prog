// Fill out your copyright notice in the Description page of Project Settings.


#include "KingHUD.h"

void AKingHUD::DrawHUD()
{
	Super::DrawHUD(); // 必须先调用父类

	// ======================
	// Draw2DLine 标准用法
	// ======================

	// 1. 画一条 屏幕中间 横线（白色）
	Draw2DLine(
		200,        // X1
		300,        // Y1
		600,        // X2
		300,        // Y2
		FColor::White // 颜色
	);

	// 2. 画一条 红色竖线
	Draw2DLine(500, 100, 500, 500, FColor::Red);

	// 3. 半透明蓝色线（A=100 透明）
	Draw2DLine(100, 100, 800, 500, FColor(0, 0, 255, 100));
}