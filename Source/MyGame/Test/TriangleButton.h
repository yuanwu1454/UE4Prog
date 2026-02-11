// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Input/HittestGrid.h"
#include "Layout/ArrangedWidget.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class MYGAME_API STriangleButton : public SButton
{
public:
	SLATE_BEGIN_ARGS(STriangleButton):
		_TriangleColor(FLinearColor::White)
		, _HoveredColor(FLinearColor::Gray)
		, _PressedColor(FLinearColor::Black)
		, _TriangleSize(FVector2D(40.f, 40.f))
	{}

	SLATE_ARGUMENT(FLinearColor, TriangleColor);
	SLATE_ARGUMENT(FLinearColor, HoveredColor);
	SLATE_ARGUMENT(FLinearColor, PressedColor);
	SLATE_ARGUMENT(FVector2D, TriangleSize);
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

protected:
	// 重写绘制逻辑
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	
	// 获取当前应该使用的颜色（根据按钮状态）
	FLinearColor GetCurrentTriangleColor() const;
private:
	// 按钮状态对应的颜色属性
	TAttribute<FLinearColor> TriangleColor;
	TAttribute<FLinearColor> HoveredColor;
	TAttribute<FLinearColor> PressedColor;
	// 三角形尺寸
	TAttribute<FVector2D> TriangleSize;

	FReply OnTriClicked();
	class FCustomTriangleHitTestPath;
};



// 基于UE4.27原生ICustomHitTestPath的正确实现
class STriangleButton::FCustomTriangleHitTestPath : public ICustomHitTestPath
{
public:
    // 构造函数：传入当前按钮、三角形顶点（本地归一化坐标）、控件几何
    FCustomTriangleHitTestPath(TSharedRef<STriangleButton> InOwnerButton, const TArray<FVector2D>& InTriangleNormalVerts, const FGeometry& InWidgetGeometry)
        : OwnerButton(InOwnerButton)
        , TriangleNormalVerts(InTriangleNormalVerts)
        , WidgetGeometry(InWidgetGeometry)
    {}

    // -------------------------- 核心：自定义命中路径（决定是否命中） --------------------------
    virtual TArray<FWidgetAndPointer> GetBubblePathAndVirtualCursors(
        const FGeometry& InGeometry, 
        FVector2D DesktopSpaceCoordinate, 
        bool bIgnoreEnabledStatus
    ) const override
    {
        TArray<FWidgetAndPointer> HitPath;

        // 1. 转换鼠标坐标：桌面空间 → 控件本地像素坐标
        // 先转窗口空间（Desktop→Window）

    	FVector2D ScreenSpaceCoord = FSlateApplication::Get().GetCursorPos(); // 直接获取鼠标屏幕坐标（替代手动转换）


    	// 1. 坐标转换（UE4.27 原生）
    	FVector2D CurrentMousePos = FSlateApplication::Get().GetCursorPos(); // 当前鼠标屏幕坐标
    	FVector2D LastMousePos = FSlateApplication::Get().GetLastCursorPos(); // 上一帧鼠标坐标（4.27支持）
    	FVector2D AbsoluteMousePos = WidgetGeometry.LocalToAbsolute(CurrentMousePos);
    	FVector2D LocalMousePos = WidgetGeometry.AbsoluteToLocal(AbsoluteMousePos);
    	

    	// -------------------------- 三角形命中判断（逻辑不变） --------------------------
    	const FVector2D WidgetSize = WidgetGeometry.GetLocalSize();
    	TArray<FVector2D> TrianglePixelVerts;
    	for (const FVector2D& NormalVert : TriangleNormalVerts)
    	{
    		TrianglePixelVerts.Add(FVector2D(
				NormalVert.X * WidgetSize.X,
				NormalVert.Y * WidgetSize.Y
			));
    	}

    	// 仅当鼠标在三角形内时，返回命中路径
    	if (IsPointInTriangle(LocalMousePos, TrianglePixelVerts))
    	{

    		// 步骤1：构造FArrangedWidget（排列后的控件，核心）
    		FArrangedWidget ArrangedWidget(OwnerButton, WidgetGeometry);
            
    		// 步骤2：构造FVirtualPointerPosition（虚拟指针位置）
    		TSharedPtr<FVirtualPointerPosition> VirtualPointer = MakeShared<FVirtualPointerPosition>();
    		VirtualPointer->CurrentCursorPosition = CurrentMousePos; // 鼠标屏幕坐标
    		VirtualPointer->LastCursorPosition = LastMousePos; // 桌面坐标

    		// 步骤3：构造FWidgetAndPointer（UE4.27 正确方式）
    		FWidgetAndPointer WidgetAndPointer(ArrangedWidget, VirtualPointer);
            
    		// 步骤4：添加到命中路径
    		HitPath.Add(WidgetAndPointer);
    	}

        // 未命中：返回空数组，不触发任何交互
        return HitPath;
    }

    // -------------------------- 无嵌套子控件，空实现即可 --------------------------
    virtual void ArrangeCustomHitTestChildren(FArrangedChildren& ArrangedChildren) const override
    {
        // 当前按钮无子控件，无需排列，空实现
    }

    // -------------------------- 无子控件，返回空即可 --------------------------
    virtual TSharedPtr<FVirtualPointerPosition> TranslateMouseCoordinateForCustomHitTestChild(
        const TSharedRef<SWidget>& ChildWidget, 
        const FGeometry& ViewportGeometry, 
        const FVector2D& ScreenSpaceMouseCoordinate, 
        const FVector2D& LastScreenSpaceMouseCoordinate
    ) const override
    {
        // 无子控件，返回空
        return nullptr;
    }

private:
    // 所属按钮的引用
    TSharedRef<STriangleButton> OwnerButton;
    // 三角形归一化顶点（0~1）
    TArray<FVector2D> TriangleNormalVerts;
    // 控件几何信息
    FGeometry WidgetGeometry;

    // 复用：叉乘法判断点是否在三角形内
    bool IsPointInTriangle(const FVector2D& Point, const TArray<FVector2D>& TriangleVertices) const
    {
        check(TriangleVertices.Num() == 3);
        const FVector2D& A = TriangleVertices[0];
        const FVector2D& B = TriangleVertices[1];
        const FVector2D& C = TriangleVertices[2];

        bool b1 = ((B.X - A.X) * (Point.Y - A.Y) - (B.Y - A.Y) * (Point.X - A.X)) < 0.0f;
        bool b2 = ((C.X - B.X) * (Point.Y - B.Y) - (C.Y - B.Y) * (Point.X - B.X)) < 0.0f;
        bool b3 = ((A.X - C.X) * (Point.Y - C.Y) - (A.Y - C.Y) * (Point.X - C.X)) < 0.0f;

        return ((b1 == b2) && (b2 == b3));
    }
};