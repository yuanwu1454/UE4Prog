// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctinLibrary.h"

#include "Slate/MyMenuAnchor.h"
#include "Test/SharePtrTest.h"
#include "Widgets/SWindow.h"
#include "Widgets/Layout/SPopup.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SConstraintCanvas.h"

static TWeakPtr<SWindow> SPopupWeakPtr;
static TWeakPtr<SWindow> SSimpleWindowWeakPtr;
TMap<FName, TWeakPtr<SWidget>> UMyBlueprintFunctinLibrary::WidgetCache = {};
static bool bClickeded = false;

// 创建带边框/关闭按钮的 SPopupWindow
void ShowCustomPopupWindow();

class SButtonInteractionWidget : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SButtonInteractionWidget){}
    SLATE_END_ARGS()
    void Construct(const FArguments& InArgs)
    {
        ChildSlot[
            SNew(SBox)
            .VAlign(VAlign_Center)
            .HAlign(HAlign_Center)
            // .WidthOverride(500.f)
            .MinDesiredWidth(500.f)
            [
                SNew(SVerticalBox)
                +SVerticalBox::Slot()
                [
                    SAssignNew(TextBlockPtr, STextBlock)
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                ]
                +SVerticalBox::Slot()
                [
                    SAssignNew(ButtonPtr, SButton)
                    .OnClicked(this, &SButtonInteractionWidget::OnButtonClicked)
                ]
                +SVerticalBox::Slot()
                [
                    SAssignNew(EditableTextBoxPtr, SEditableTextBox)
                    .OnTextChanged(this, &SButtonInteractionWidget::OnEditableTextBoxChanged)
                    .HintText(FText::FromString(TEXT("请输入内容...")))
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                ]
                +SVerticalBox::Slot()
                .Padding(0,15,0,0)
                [
                    SAssignNew(TextBlock2Ptr, STextBlock)
                    .Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
                ]
            ]
        ];
        UpdateUI();
    }

private:
    TSharedPtr<STextBlock> TextBlockPtr;
    TSharedPtr<SButton> ButtonPtr;
    TSharedPtr<SEditableTextBox> EditableTextBoxPtr;
    TSharedPtr<STextBlock> TextBlock2Ptr;

    bool bClicked = false;
    FText ClickedText = FText::FromString(TEXT("按钮已经点击"));
    FText UnClickedText = FText::FromString(TEXT("按钮未点击"));
    FSlateColor ClickedColor = FSlateColor(FLinearColor(0,0.8,0.0,1.0));
    FSlateColor UnClickedColor = FSlateColor::UseForeground();

    FText GetStatusText() const
    {
        return bClicked ? ClickedText : UnClickedText;
    }
    FSlateColor GetButtonColor() const
    {
        return bClicked ? ClickedColor : UnClickedColor;
    }
    
    void UpdateUI()
    {
        if (TextBlockPtr&&TextBlockPtr.IsValid())
        {
            TextBlockPtr->SetText(GetStatusText());
        }

        if (ButtonPtr&&ButtonPtr.IsValid())
        {
            ButtonPtr->SetBorderBackgroundColor(GetButtonColor());
        }
    }
    FReply OnButtonClicked()
    {
        bClicked = !bClicked;
        UpdateUI();
        return FReply::Handled();
    }

    void OnEditableTextBoxChanged(const FText& Text)
    {
        if (TextBlock2Ptr && TextBlock2Ptr.IsValid())
        {
            TextBlock2Ptr->SetText(Text);
        }
    }
};

void ShowSimpleWindow()
{
    if (auto Ref =SSimpleWindowWeakPtr.Pin())
    {
        if (Ref.IsValid())
        {
            UE_LOG(LogTemp, Log, TEXT("Simple Windows is Exists!"));
            return;
        }
    }
    //
    // auto Content = SNew(SBox)
    // .HAlign(HAlign_Center)
    // .VAlign(VAlign_Center)
    // [
    //     SNew(SVerticalBox)
    //     +SVerticalBox::Slot()
    //     [
    //         SNew(STextBlock).Text(FText::FromString(TEXT("Slate基础布局练习")))
    //     ]
    //     +SVerticalBox::Slot()
    //     [
    //         SNew(STextBlock).Text(FText::FromString(TEXT("点击下方按钮")))
    //     ]
    //     +SVerticalBox::Slot()
    //     [
    //         SNew(SBox)
    //         .MinDesiredWidth(100)
    //         .MinDesiredHeight(30)
    //         .Content()
    //         [
    //             SNew(SButton)
    //             .Content()[SNew(STextBlock).Text(FText::FromString(TEXT("确认")))]
    //         ]
    //     ]
    // ];

    // auto Content = SNew(SBox)
    // .HAlign(HAlign_Center)
    // .VAlign(VAlign_Center)
    // .Content()
    // [
    //     SNew(STextBlock)
    //     .Text(FText::FromString(TEXT("Slate 文本样式练习")))
    //     .Font(FCoreStyle::GetDefaultFontStyle("Regular", 24))
    //     .Justification(ETextJustify::Center)
    //     .LineHeightPercentage(2.0f)
    //     .WrapTextAt(600.f)
    //     .ColorAndOpacity(FLinearColor(0,0,0.5f,1))
    //     .ShadowOffset(FVector2D(-2,-2))
    //     .ShadowColorAndOpacity(FLinearColor(0.8f,0.8f,0.8f,1))
    // ];

    auto Content = SNew(SBox)
    .HAlign(HAlign_Center)
    .VAlign(VAlign_Center)
    .MinDesiredHeight(40.f)
    .MinDesiredWidth(120.f)
    .Content()
    [
        SNew(SButton)
        .OnClicked_Lambda([]()
        {
            bClickeded = !bClickeded;
            return FReply::Handled();
        })
        .ButtonColorAndOpacity_Lambda([]()
        {
            return bClickeded?FLinearColor(0.f,0.8f,0.f,1.0f):FLinearColor(1.f,1.f,1.f,1.0f);            
        })
        .Content()
        [
            SNew(STextBlock)
            .Text_Lambda([]()
            {
                return bClickeded?FText::FromString(TEXT("按钮已经点击")):FText::FromString(TEXT("按钮未点击"));
            })
        ]
    ];
    
    // TSharedRef<SWindow> SimpleWindow = SNew(SWindow)
    // .Title(FText::FromString(TEXT("SimpleWindow")))
    // .AutoCenter(EAutoCenter::PrimaryWorkArea)
    // .ClientSize(FVector2D(600,400))
    // .Content()[Content->AsShared()];

    TSharedRef<SWindow> SimpleWindow = SNew(SWindow)
    .Title(FText::FromString(TEXT("SimpleWindow")))
    .ClientSize(FVector2D(800, 600)) // 窗口大小
    .SupportsMinimize(false)
    .SupportsMaximize(false);

    SimpleWindow->SetContent(SNew(SButtonInteractionWidget));

    if (FSlateApplication::IsInitialized())
    {
        FSlateApplication::Get().AddWindow(SimpleWindow);
        SSimpleWindowWeakPtr = SimpleWindow;        
    }
}

void UMyBlueprintFunctinLibrary::Test(UWorld* Obj)
{
	SharePtrTest::Test(Obj);
}

void UMyBlueprintFunctinLibrary::ShowPopWindow()
{
    // ShowCustomPopupWindow();
    ShowSimpleWindow();
}

void UMyBlueprintFunctinLibrary::TestMyMenuAnchor(UWorld* Obj)
{
    // 1. 获取 视口客户端
    UGameViewportClient* ViewportClient = Obj->GetGameViewport();
    if (!ViewportClient) return;

    FName Key = FName("MyMenu");
    if (WidgetCache.Contains(Key))
    {
        // 2. 再看控件是否有效
        if (WidgetCache[Key].IsValid())
        {
            // 3. 找到了 → 不重复创建
            UE_LOG(LogTemp, Warning, TEXT("控件已存在"));
            return;
        }else
        {
            // 控件无效，清理脏数据
            WidgetCache.Remove(Key);
        }
    }

    // 2. 用 SNew 创建你的 Slate 控件
    TSharedRef<SWidget> MySlateWidget = SNew(SConstraintCanvas)
        + SConstraintCanvas::Slot()
    .Offset( FMargin(
    500,   // X 坐标（左边距）
    200,   // Y 坐标（上边距）
    300,   // 宽度
    150    // 高度
) ) // X=0, Y=0, W=1920, H=1080
        [
        SNew(SMyMenuAnchorTestWidget)
        ];

    // 3. ✅ 关键：添加到视口（最标准写法）
    ViewportClient->AddViewportWidgetContent(MySlateWidget, 2);

    WidgetCache.Add(Key, MySlateWidget->AsShared());
}


void ShowCustomPopupWindow()
{
    if (TSharedPtr<SWindow> StrongWindow = SPopupWeakPtr.Pin())
    {
        if (StrongWindow.IsValid())
        {
            UE_LOG(LogTemp, Log, TEXT("Windows is Exists!"));
            return;
        }
    }

    // Slate 框架持有对象后，其引用计数才不会 “无故归零”，TWeakPtr 的校验才有意义。
       // 1. 创建 SWindow 并开启弹窗模式（核心：IsPopupWindow=true）
    TSharedRef<SWindow> PopupWindow = SNew(SWindow)
        // 核心属性：开启弹窗模式（等同于 UE5 的 SPopupWindow）
        .IsPopupWindow(true)
        // 弹窗基础配置（模拟 SPopupWindow 的默认行为）
        .Title(FText::FromString(TEXT("UE4.27 弹窗式 SWindow")))
        .ClientSize(FVector2D(400, 250)) // 弹窗尺寸
        .SizingRule(ESizingRule::FixedSize) // 固定尺寸（不可拖动调整）
        .FocusWhenFirstShown(true) // 显示时自动获取焦点
        .AutoCenter(EAutoCenter::PreferredWorkArea);



    // 2. 显示弹窗（UE4.27 必须通过 SlateApplication 添加）
    if (FSlateApplication::IsInitialized())
    {
        PopupWindow->GetOnWindowClosedEvent().AddLambda([]( const TSharedRef<SWindow>&)
        {
            UE_LOG(LogTemp, Log, TEXT("弹窗已关闭"));
        });
        FSlateApplication::Get().AddWindow(PopupWindow);
    }
    
    
    // 2. 手动构建弹窗内容（关键：AddWindow 后再绑定 Lambda）
    TSharedRef<SVerticalBox> PopupContent = SNew(SVerticalBox)
        // 标题栏 + 关闭按钮
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
            // 标题文本
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("UE4.27 自定义弹窗")))
                .ColorAndOpacity(FLinearColor::White)
                .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 14))
            ]
            // 关闭按钮（此时 PopupWindow 已被正确初始化）
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Close"))
                .OnClicked_Lambda([WeakWindow = TWeakPtr<SWindow>(PopupWindow)]()
                {
                    if (TSharedPtr<SWindow> StrongWindow = WeakWindow.Pin())
                    {
                        if (StrongWindow.IsValid() && FSlateApplication::IsInitialized())
                        {
                            FSlateApplication::Get().RequestDestroyWindow(StrongWindow.ToSharedRef());
                        }
                    }
                    return FReply::Handled();
                })
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(TEXT("X")))
                    .ColorAndOpacity(FLinearColor::White)
                ]
            ]
        ]
        // 内容区
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .Padding(10)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("修复 WeakPtr 绑定的弹窗！")))
            .ColorAndOpacity(FLinearColor::White)
            .Font(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12))
            .WrapTextAt(350)
            .Justification(ETextJustify::Center)
        ];


    // 3. 设置窗口内容
    SPopupWeakPtr = PopupWindow;
    PopupWindow->SetContent(PopupContent);
}
