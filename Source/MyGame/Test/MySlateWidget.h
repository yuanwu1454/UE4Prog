// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWeakWidget.h"




using FunType = int32(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

// const 修饰的函数类型不能声明普通指针
// using FunPtrType = FunType*;
using PaintFuncType = TFunction<int32(const FPaintArgs&, const FGeometry&, const FSlateRect&, 
									 FSlateWindowElementList&, int32, const FWidgetStyle&, bool)>;
/**
 * 
 */
class MYGAME_API SMySlateWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMySlateWidget):_WidgetTitle(FText::FromString(TEXT("我的Slate控件")))
	{}
	SLATE_ARGUMENT(FText, WidgetTitle)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	~SMySlateWidget();
	static TWeakPtr<SMySlateWidget> GMySlateWidgetRoot;
	static void TestCreateWidget();
	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	

	FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
private:
	// 【可选】子控件句柄：保存子控件的引用，方便后续交互（如按钮点击、文本修改）
	TSharedPtr<STextBlock> TitleText;    // 标题文本控件
	TSharedPtr<SButton> TestButton;      // 测试按钮控件

	// 【可选】按钮点击的回调函数
	FReply OnTestButtonClicked();

	// 存储外部传入的标题（私有成员）
	FText WidgetTitle;
	int32 RenderIndex=0;


	TArray<PaintFuncType> FuncArr;

	FunType CustomPaint;
	FunType CustomPaint1;
	PaintFuncType CustomPaintType1;
	PaintFuncType CustomPaintType2;
	FLinearColor CircleColor = FLinearColor::Green;
};


