#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SMenuAnchor.h"
#include "Framework/Application/IMenu.h"

/**
 * 自定义 MenuAnchor，完全继承引擎原生 SMenuAnchor
 * 可重写：打开/关闭、菜单位置、菜单逻辑
 */
class SLATE_API SMyMenuAnchor : public SMenuAnchor
{
public:
    SLATE_BEGIN_ARGS(SMyMenuAnchor)
        : _Content()
        , _Padding(FMargin(0.f))
        , _MenuContent(SNew(STextBlock).Text(NSLOCTEXT("SMyMenuAnchor", "NoMenuContent", "No Menu Content Assigned; use .MenuContent")))
        , _OnGetMenuContent()
        , _OnMenuOpenChanged()
        , _Placement(MenuPlacement_BelowAnchor)
        , _FitInWindow(true)
        , _Method()
        , _ShouldDeferPaintingAfterWindowContent(true)
        , _UseApplicationMenuStack(true)
        , _IsCollapsedByParent(false)
        , _ApplyWidgetStyleToMenu(true)
    {}

    SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_ARGUMENT(FMargin, Padding)
    SLATE_ARGUMENT(TSharedPtr<SWidget>, MenuContent)
    SLATE_EVENT(FOnGetContent, OnGetMenuContent)
    SLATE_EVENT(FOnIsOpenChanged, OnMenuOpenChanged)
    SLATE_ATTRIBUTE(EMenuPlacement, Placement)
    SLATE_ARGUMENT(bool, FitInWindow)
    SLATE_ARGUMENT(TOptional<EPopupMethod>, Method)
    SLATE_ARGUMENT(bool, ShouldDeferPaintingAfterWindowContent)
    SLATE_ARGUMENT(bool, UseApplicationMenuStack)
    SLATE_ARGUMENT(bool, IsCollapsedByParent)
    SLATE_ARGUMENT(bool, ApplyWidgetStyleToMenu)

SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // ========== 你可以自由重写的核心虚函数 ==========
    virtual void SetIsOpen(bool InIsOpen, const bool bFocusMenu = true, const int32 FocusUserIndex = 0) override;
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
private:
    // 自定义标记
    bool bIsMyMenuOpening = false;
};

// 👇 这是一个可直接运行的测试控件，直接用它即可
class SMyMenuAnchorTestWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMyMenuAnchorTestWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs)
    {
        ChildSlot
        [
            SNew(SMyMenuAnchor)
            .MenuContent
            (
                SNew(SBorder)
                .BorderBackgroundColor(FColor::Black)
                .Padding(10)
                [
                    SNew(STextBlock).Text(FText::FromString(TEXT("菜单内容")))
                ]
            )
            [
                SNew(SButton).Text(FText::FromString(TEXT("点我弹出菜单")))
            ]
        ];
    }
};