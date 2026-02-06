// Fill out your copyright notice in the Description page of Project Settings.


#include "MySlateWidget.h"

#include "SlateOptMacros.h"
#include "Widgets/Images/SImage.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedPtr<SMySlateWidget> SMySlateWidget::GMySlateWidgetRoot = nullptr;
void SMySlateWidget::Construct(const FArguments& InArgs)
{
	// 1. 获取外部传入的参数，赋值给私有成员
	WidgetTitle = InArgs._WidgetTitle;
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
		.VAlign(VAlign_Bottom)
		.Padding(20)
		.AutoHeight()[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)      // 垂直居上
			.Padding(0)
			.FillWidth(0.5)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor(FLinearColor(1.0,0.0,0.0)))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)      // 垂直居上
			.Padding(0)
			.FillWidth(0.5)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor(FLinearColor(0.0,1.0,0.0)))
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)      // 垂直居上
			.FillWidth(1.0)
			.Padding(0)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor(FLinearColor(0.0,0.0,1.0)))
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

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
