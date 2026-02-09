// Fill out your copyright notice in the Description page of Project Settings.


#include "MySlateWidget.h"

#include "SlateOptMacros.h"
#include "Input/HittestGrid.h"
#include "Widgets/SToolTip.h"
#include "Widgets/SViewport.h"
#include "Widgets/Images/SImage.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedPtr<SMySlateWidget> SMySlateWidget::GMySlateWidgetRoot = nullptr;
void SMySlateWidget::Construct(const FArguments& InArgs)
{
	// 1. 获取外部传入的参数，赋值给私有成员
	WidgetTitle = InArgs._WidgetTitle;
	RenderIndex = 0;

	CustomPaintType1 = [this](const FPaintArgs& Args, 
	   const FGeometry& AllottedGeometry,
	   const FSlateRect& MyCullingRect, 
	   FSlateWindowElementList& OutDrawElements, 
	   int32 LayerId,
	   const FWidgetStyle& InWidgetStyle, 
	   bool bParentEnabled)
	{
		return this->CustomPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	};

	CustomPaintType2 = [this](const FPaintArgs& Args, 
	   const FGeometry& AllottedGeometry,
	   const FSlateRect& MyCullingRect, 
	   FSlateWindowElementList& OutDrawElements, 
	   int32 LayerId,
	   const FWidgetStyle& InWidgetStyle, 
	   bool bParentEnabled)
		{
			return this->CustomPaint1(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		};
	
	FuncArr.EmplaceAt(0, CustomPaintType1);
	FuncArr.EmplaceAt(1, CustomPaintType2);
	// 2. 构建UI布局：Slate采用**链式调用+声明式**编写，核心是SNew（创建Slate控件）
	ChildSlot
	// 根布局：SVerticalBox（垂直布局，子控件从上到下排列），Slate常用布局还有SHorizontalBox（水平）、SGridPanel（网格）
	[
		SNew(SVerticalBox)
		// 垂直布局的第1个槽位：添加标题文本
		+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)    // 水平居中
			.VAlign(VAlign_Top)      // 垂直居上
			.Padding(20)             // 内边距（上下左右20像素）
			.AutoHeight()            // 高度自适应内容
		[
			// 创建文本控件STextBlock，并用TSharedPtr保存引用
			SAssignNew(TitleText, STextBlock)
				.Text(WidgetTitle)   // 设置文本内容
				.Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 24)) // 字体+字号
				.ColorAndOpacity(FLinearColor::White) // 文本颜色（白色）
		]
		// 垂直布局的第2个槽位：添加测试按钮
		+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(20)
			.AutoHeight()
		[
			// 创建按钮控件SButton，并用TSharedPtr保存引用
			SAssignNew(TestButton, SButton)
				.Text(FText::FromString(TEXT("点击我测试"))) // 按钮文本
				.HAlign(HAlign_Center)                      // 按钮内文本居中
				.VAlign(VAlign_Center)
				.ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button")) // 按钮样式（用UE4默认样式）
				.OnClicked(this, &SMySlateWidget::OnTestButtonClicked) // 绑定点击回调
				// .ToolTip(
				// 	SNew(STextBlock)
				// 	.Text(FText::FromString(TEXT("这是Slate按钮的悬停提示"))))
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(0)
		.FillHeight(1.0)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)      // 垂直居上
			.Padding(0)
			.FillWidth(0.5)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor(FLinearColor(1.0,0.0,0.0)))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Fill)      // 垂直居上
			.Padding(0)
			.FillWidth(0.5)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor(FLinearColor(0.0,1.0,0.0)))
				.ToolTip(SNew(SToolTip)
				// .Text(
				// 	FText::FromString(
				// 		FString::Printf(TEXT("LOL=%s"), TEXT("xzcv"))
				// 	)
				// )
				
				.Text(FText::Format(
					FText::FromString(TEXT("LOL={0}")),
						FText::FromString(TEXT("zxcv"))
					)
					)
				
					)
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)      // 垂直居上
			.FillWidth(0.5)
			.Padding(0)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor(FLinearColor(0.0,0.0,1.0)))
				.ToolTip(
					SNew(SToolTip)
					// 可选：设置ToolTip的显示延迟（默认0.5秒，可自定义）
					// .Text(FText::FromString(TEXT("这是SImage的悬浮提示")))
					.Text_Lambda([]()
					{
						FVector2D MousePos = FSlateApplication::Get().GetCursorPos();
						FString DebugMessage = FString::Printf(TEXT("MousePos: X=%.2f, Y=%.2f"), MousePos.X, MousePos.Y);
						// 动态文本用FText::Format，而非直接拼接
						return FText::FromString(DebugMessage);
					})
				)
			]

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)      // 垂直居上
			.FillWidth(0.5)
			.Padding(0)
			[
				SNew(SImage)
				// .ColorAndOpacity(FSlateColor(FLinearColor(1.0,0.0,1.0)))
				.ColorAndOpacity_Lambda([]()
				{
					FVector2D MousePos = FSlateApplication::Get().GetCursorPos();
					// 2. 获取屏幕分辨率（用于将鼠标位置映射到0-1范围）
					auto ViewPort = FSlateApplication::Get().GetGameViewport();
				    FVector2D ScreenSize = ViewPort->GetDesiredSize();
					// 3. 防止除零错误（屏幕尺寸至少为1）
					float ScreenWidth = FMath::Max(ScreenSize.X, 1.0f);
					float ScreenHeight = FMath::Max(ScreenSize.Y, 1.0f);
					        
					// 4. 将鼠标X/Y坐标映射到0-1的RGB值范围
					// X轴控制红色通道，Y轴控制绿色通道，固定蓝色通道为0.5
					float Red = FMath::Clamp(MousePos.X / ScreenWidth, 0.0f, 1.0f);
					float Green = FMath::Clamp(MousePos.Y / ScreenHeight, 0.0f, 1.0f);
					float Blue = 0.5f; // 固定蓝色，也可改为随位置变化
					float Alpha = 1.0f; // 不透明

					// if (GEngine)
					// {
					// 	// 方式1：推荐 - 使用FString::Printf拼接数值
					// 	FString DebugMessage = FString::Printf(TEXT("MousePos: X=%.2f, Y=%.2f"), MousePos.X, MousePos.Y);
					// 	    
					// 	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, DebugMessage);
					// }
					//
					// 5. 返回根据鼠标位置计算的颜色
					return FSlateColor(FLinearColor(Red, Green, Blue, Alpha));
				})
			]
		]
	];

	// 绑定全局退出回调：游戏关闭时执行
}

SMySlateWidget::~SMySlateWidget()
{
	UE_LOG(LogTemp, Log, TEXT("SMySlateWidget Desctruct"));
}

// 实现按钮点击的回调函数
FReply SMySlateWidget::OnTestButtonClicked()
{
	// 1. 打印日志（测试回调是否生效）
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Slate按钮被点击了！"));
	}

	// 2. 【可选】修改文本控件的内容（演示子控件交互）
	if (TitleText.IsValid())
	{
		TitleText->SetText(FText::FromString(TEXT("按钮已点击！")));
	}

	RenderIndex += 1;
	if(!FuncArr.IsValidIndex(RenderIndex))
	{
		RenderIndex = 0;
	}
	// FReply::Handled() 表示处理了此次点击事件，不再向下传递
	return FReply::Handled();
}

void SMySlateWidget::TestCreateWidget()
{
	UE_LOG(LogTemp, Log, TEXT("TestCreateWidget"));

	if (SMySlateWidget::GMySlateWidgetRoot.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("smyslatewidget is exists"))
		return;
	}

	// 1. 先创建强引用的根控件（局部强引用，用于构建和挂载）
	SMySlateWidget::GMySlateWidgetRoot = SNew(SMySlateWidget)
		.WidgetTitle(FText::FromString(TEXT("My Create Wiget")));

	// 2. 挂载到视口
	GWorld->GetGameViewport()->AddViewportWidgetContent(
		SNew(SWeakWidget)
			.PossiblyNullContent(SMySlateWidget::GMySlateWidgetRoot)
	,10);
}

int32 SMySlateWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (FuncArr.IsValidIndex(RenderIndex))
	{
		return FuncArr[RenderIndex](Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
									bParentEnabled);
	}

	int32 NewLayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
										bParentEnabled);
	return NewLayerId ;
}



END_SLATE_FUNCTION_BUILD_OPTIMIZATION
