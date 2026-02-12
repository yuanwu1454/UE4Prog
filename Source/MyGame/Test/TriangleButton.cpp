// Fill out your copyright notice in the Description page of Project Settings.


#include "TriangleButton.h"

#include "SlateOptMacros.h"
#include "Input/HittestGrid.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

bool STriangleButton::FCustomTriangleHitTestPath::bPrintTriPos = false;

void STriangleButton::Construct(const FArguments& InArgs)
{
	// 绑定属性
	TriangleColor = InArgs._TriangleColor;
	HoveredColor = InArgs._HoveredColor;
	PressedColor = InArgs._PressedColor;
	TriangleSize = InArgs._TriangleSize;
	ButtonPtr = InArgs._ButtonPtr;

	// 调用父类构造
	SButton::Construct(SButton::FArguments()
		// 禁用父类默认的背景绘制，避免覆盖我们的三角形
		.ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
		.OnClicked(this, &STriangleButton::OnTriClicked)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
	);
}


int32 STriangleButton::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
	const FLinearColor CurrentColor = GetCurrentTriangleColor() * (bIsEnabled ? 1.0f : 0.5f); // 禁用时半透明

	// Args.GetHittestGrid().RemoveWidget(this);



	// 获取三角形尺寸
	const FVector2D Size = TriangleSize.Get();
	// 计算三角形的三个顶点（以按钮中心为原点，绘制向上的三角形，可根据需求修改顶点坐标）
	const FVector2D Center = AllottedGeometry.GetLocalSize() / 2.0f;
	
	const FVector2D LocalSize = AllottedGeometry.GetLocalSize(); // 控件本地像素尺寸
	const FVector2D LocalCenter_Pixel = LocalSize / 2.0f; // 控件中心（像素坐标）

	// -------------------------- 核心修正：像素坐标 → 归一化坐标 --------------------------
	// 步骤1：计算三角形顶点的像素坐标（控件本地）
	TArray<FVector2D> TrianglePoints_Pixel;
	TrianglePoints_Pixel.Add(LocalCenter_Pixel + FVector2D(0, -Size.Y / 2)); // 顶部
	TrianglePoints_Pixel.Add(LocalCenter_Pixel + FVector2D(-Size.X / 2, Size.Y / 2)); // 左下角
	TrianglePoints_Pixel.Add(LocalCenter_Pixel + FVector2D(Size.X / 2, Size.Y / 2)); // 右下角

	// 步骤2：转换为归一化坐标（0~1）
	TArray<FVector2D> TrianglePoints_Normal;
	for (const FVector2D& PixelPos : TrianglePoints_Pixel)
	{
		// 关键：像素坐标 / 控件尺寸 = 归一化坐标
		TrianglePoints_Normal.Add(FVector2D(
			PixelPos.X / LocalSize.X,
			PixelPos.Y / LocalSize.Y
		));
	}

	TSharedRef<const SWidget> s = this->AsShared();
	TSharedRef<SWidget> s2 = ConstCastSharedRef<SWidget>(s);
	TSharedRef<STriangleButton>s3 = StaticCastSharedRef<STriangleButton>(s2);
	if(!TestPathPtr.IsValid())
	{
		

		// OnPaint中强制转换（仅确保控件可修改时使用）
		// 步骤1：创建自定义HitTestPath的共享指针实例（传入必要参数）
		TSharedPtr<STriangleButton::FCustomTriangleHitTestPath> CustomHitTestPath = 
			MakeShared<STriangleButton::FCustomTriangleHitTestPath>(
				s3,       // 当前按钮的TSharedRef（转TSharedPtr）
				TrianglePoints_Normal,  // 三角形归一化顶点（用于命中判断）
				AllottedGeometry        // 控件几何信息（用于坐标转换）
			);
		TestPathPtr = CustomHitTestPath;
	}else
	{
		TestPathPtr->SetWidgetGeometry(AllottedGeometry);
		TestPathPtr->SetTriangleNormalVerts(TrianglePoints_Normal);
	}
	
	TSharedRef<FCustomTriangleHitTestPath>  TriangleHitTestRef = TestPathPtr.ToSharedRef();

	// 3. 第三步：向上转换为TSharedRef<ICustomHitTestPath>
	// 利用多态自动向上转换，无需额外强制转换（类型兼容）
	TSharedRef<ICustomHitTestPath> HitTestRef = TriangleHitTestRef;

	Args.GetHittestGrid().InsertCustomHitTestPath(s3, HitTestRef);

	TArray<FSlateVertex> Verts;
	// const FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry();
	
	// 顶点1：顶部
	Verts.AddZeroed();
	{
		FSlateVertex& NewVert = Verts.Last();
		NewVert.Position = AllottedGeometry.GetAbsolutePositionAtCoordinates(TrianglePoints_Normal[0]);
		NewVert.TexCoords[0] =NewVert.TexCoords[1] = 0.f;
		NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
		NewVert.Color = CurrentColor.ToFColor(false);
		NewVert.PixelSize[0] = 2;
		NewVert.PixelSize[1] = 9;

	}
	Verts.AddZeroed();
	{
		FSlateVertex& NewVert = Verts.Last();
		NewVert.Position = AllottedGeometry.GetAbsolutePositionAtCoordinates(TrianglePoints_Normal[1]);
		NewVert.TexCoords[0] =NewVert.TexCoords[1] = 0.f;
		NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
		NewVert.Color = CurrentColor.ToFColor(false);
		NewVert.PixelSize[0] = 3;
		NewVert.PixelSize[1] = 8;

	}

	Verts.AddZeroed();
	{
		FSlateVertex& NewVert = Verts.Last();

		NewVert.Position = AllottedGeometry.GetAbsolutePositionAtCoordinates(TrianglePoints_Normal[2]);
		NewVert.TexCoords[0] =NewVert.TexCoords[1] = 0.f;
		NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
		NewVert.Color = CurrentColor.ToFColor(false);
		NewVert.PixelSize[0] = 4;
		NewVert.PixelSize[1] = 7;
	}
	
	TArray<SlateIndex> Indexes;
	Indexes.Add(0);
	Indexes.Add(1);
	Indexes.Add( 2);
	

	const FSlateBrush* MyBrush = FCoreStyle::Get().GetBrush("ColorWheel.HueValueCircle");

	FSlateResourceHandle Handle = MyBrush->GetRenderingResource();
	// 绘制填充的三角形
	FSlateDrawElement::MakeCustomVerts(
		OutDrawElements,
		LayerId,
		FSlateResourceHandle(),
		Verts,
		Indexes, // 无纹理
		nullptr,
		0,    // 纹理UV偏移
		1,
		ESlateDrawEffect::None 
	);

	// 绘制三角形边框（可选）
	// FSlateDrawElement::MakeLines(
	// 	OutDrawElements,
	// 	LayerId + 1, // 边框层级高于填充，避免被覆盖
	// 	AllottedGeometry.ToPaintGeometry(),
	// 	TrianglePoints,
	// 	ESlateDrawEffect::None,
	// 	FLinearColor::Black, // 边框颜色
	// 	true, // 闭合线条（三角形是闭合图形）
	// 	2.0f  // 边框宽度
	// );

	// 调用父类绘制（处理按钮的交互反馈，如点击动画等）
	return SButton::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 2, InWidgetStyle, bParentEnabled);
}

FLinearColor STriangleButton::GetCurrentTriangleColor() const
{
	// 根据按钮状态返回对应颜色
	if (IsPressed())
	{
		return PressedColor.Get();
	}
	else if (IsHovered())
	{
		return HoveredColor.Get();
	}
	else
	{
		return TriangleColor.Get();
	}
}

FReply STriangleButton::OnTriClicked()
{
	UE_LOG(LogTemp, Log, TEXT("OnOKClicked"));
	return FReply::Handled();
}

// 重载针对 virtual修饰的函数，
// 如果重载非virtual修饰的函数，那就是重写，并且隐藏父类的函数。
FVector2D STriangleButton::ComputeDesiredSize(float LayoutScaleMultiplier) const
{

	// 方式1：返回固定尺寸（需乘以缩放系数，适配全局布局缩放）
	const float Width = 180.0f * LayoutScaleMultiplier;
	const float Height = 180.0f * LayoutScaleMultiplier;
	return FVector2D(Width, Height);
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
