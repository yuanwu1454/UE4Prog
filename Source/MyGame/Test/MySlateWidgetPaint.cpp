#include "MySlateWidget.h"

#include "SlateOptMacros.h"
#include "Input/HittestGrid.h"
#include "Widgets/SToolTip.h"
#include "Widgets/SViewport.h"
#include "Widgets/Images/SImage.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

int32 SMySlateWidget::CustomPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{

	int32 NewLayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
									bParentEnabled);

	// 2. 计算Widget的实际大小
	const FVector2D WidgetSize = AllottedGeometry.GetLocalSize();
    

	{
		// 3. 计算正方形的大小（Widget的一半）
		const float SquareSize = FMath::Min(WidgetSize.X, WidgetSize.Y) * 0.25f;
		// 4. 计算正方形的偏移（居中显示）
		const FVector2D SquareOffset = (WidgetSize - FVector2D(SquareSize, SquareSize)) * 0.25f;
		
		// 5. 定义绘制参数
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			NewLayerId + 1, // 层级比原有绘制高，确保显示在上方
			AllottedGeometry.ToPaintGeometry(FVector2D(SquareOffset), FVector2D(SquareSize)), // 绘制区域
			FCoreStyle::Get().GetBrush("WhiteBrush"), // 白色基础画刷，后续通过颜色修改为红色
			ESlateDrawEffect::None,
			FLinearColor::Red.CopyWithNewOpacity(0.4) // 红色
		);
	}


	{
		// 3. 绘制直线（核心新增逻辑）
		// 3.1 定义线段的两个端点（示例：Widget对角线，从左上角到右下角）
		TArray<FVector2D> LinePoints;
		LinePoints.Add(FVector2D(0, WidgetSize.Y/2)); // 起点：Widget左上角
		LinePoints.Add(FVector2D(WidgetSize.X, WidgetSize.Y/2)); // 终点：Widget右下角

		// 3.2 定义直线的绘制参数
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			NewLayerId + 2, // 层级比正方形高，显示在最上层
			AllottedGeometry.ToPaintGeometry(), // 使用Widget的整体几何信息
			LinePoints, // 线段的顶点数组（两个点就是一条直线）
			ESlateDrawEffect::None,
			FLinearColor::Black, // 直线颜色（蓝色，可自定义）
			true, // 是否抗锯齿（建议开启，让线条更平滑）
			10.0f // 线条宽度（单位：Slate像素，可调整）
		);
		
	}

	{
		// 3. 绘制直线（核心新增逻辑）
		// 3.1 定义线段的两个端点（示例：Widget对角线，从左上角到右下角）
		TArray<FVector2D> LinePoints;
		LinePoints.Add(FVector2D(WidgetSize.X/2, 0)); // 起点：Widget左上角
		LinePoints.Add(FVector2D(WidgetSize.X/2, WidgetSize.Y)); // 终点：Widget右下角
	
		// 3.2 定义直线的绘制参数
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			NewLayerId + 2, // 层级比正方形高，显示在最上层
			AllottedGeometry.ToPaintGeometry(), // 使用Widget的整体几何信息
			LinePoints, // 线段的顶点数组（两个点就是一条直线）
			ESlateDrawEffect::None,
			FLinearColor::Black, // 直线颜色（蓝色，可自定义）
			true, // 是否抗锯齿（建议开启，让线条更平滑）
			10.0f // 线条宽度（单位：Slate像素，可调整）
		);
	}
	{
		// 4. 绘制圆形（UE4.27兼容版，方案1：圆角矩形模拟圆形，推荐）
		const float CircleSize = FMath::Min(WidgetSize.X, WidgetSize.Y) * 0.25f; // 圆形直径
		const FVector2D CircleOffset = (WidgetSize - FVector2D(CircleSize, CircleSize)) * 0.75f; // 居中偏移

		// 4.1 创建圆角矩形画刷（圆角半径=直径/2 → 变成圆形）
		FSlateBrush CircleBrush;
		CircleBrush.SetImageSize(FVector2D(CircleSize, CircleSize));
		CircleBrush.DrawAs = ESlateBrushDrawType::Border;
		CircleBrush.Tiling = ESlateBrushTileType::NoTile;
		CircleBrush.Mirroring = ESlateBrushMirrorType::NoMirror;
		// 设置圆角半径：等于边长的一半，矩形就会变成圆形
		CircleBrush.Margin = FMargin(CircleSize / 2); 
		// 使用白色基础画刷，通过颜色参数控制最终颜色
		CircleBrush.SetResourceObject(FCoreStyle::Get().GetBrush("WhiteBrush")->GetResourceObject());

		// 4.2 绘制圆形（绿色半透明）
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			NewLayerId + 3, // 层级最高，显示在最上层
			AllottedGeometry.ToPaintGeometry(FVector2D(CircleOffset), FVector2D(CircleSize)),
			&CircleBrush,
			ESlateDrawEffect::None,
			FLinearColor::Green.CopyWithNewOpacity(0.7f) // 圆形颜色和透明度
		);
	}

	{

		// 4. 绘制圆形（UE4.27兼容版，方案1：圆角矩形模拟圆形，推荐）
		const float CircleSize = FMath::Min(WidgetSize.X, WidgetSize.Y) * 0.25f; // 圆形直径
		const FVector2D CircleOffset = (WidgetSize - FVector2D(CircleSize, CircleSize)) * 0.5f; // 居中偏移
		// 4. 绘制圆形（方案2：多边形近似圆形）
		TArray<FVector2D> CirclePoints;
		const float Radius = CircleSize / 2;
		const FVector2D CircleCenter = CircleOffset + FVector2D(Radius, Radius);
		const int32 Segments = 32; // 分段数，32段足够平滑

		// 计算多边形顶点
		for (int32 i = 0; i <= Segments; ++i)
		{
			const float Angle = 2 * PI * i / Segments;
			const float X = CircleCenter.X + Radius * FMath::Cos(Angle);
			const float Y = CircleCenter.Y + Radius * FMath::Sin(Angle);
			CirclePoints.Add(FVector2D(X, Y));
		}

		// 绘制圆形轮廓（如果要填充，需用MakeTriangles，4.27也支持）
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			NewLayerId + 3,
			AllottedGeometry.ToPaintGeometry(),
			CirclePoints,
			ESlateDrawEffect::None,
			CircleColor.CopyWithNewOpacity(0.7f),
			true,
			2.0f
		);
	}
	return NewLayerId + 3;
}


int32 SMySlateWidget::CustomPaint1(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{

	int32 NewLayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
									bParentEnabled);
	return NewLayerId ;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION