// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctinLibrary.h"
#include "Test/SharePtrTest.h"
#include "Widgets/SWindow.h"
#include "Widgets/Layout/SPopup.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"

static TWeakPtr<SWindow> SPopupWeakPtr;
// 创建带边框/关闭按钮的 SPopupWindow
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

void UMyBlueprintFunctinLibrary::Test(UWorld* Obj)
{
	SharePtrTest::Test(Obj);
}

void UMyBlueprintFunctinLibrary::ShowPopWindow()
{
    ShowCustomPopupWindow();
}
