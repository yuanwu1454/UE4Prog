#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SMenuAnchor.h"
#include "Framework/Application/IMenu.h"

// 自定义元数据
class FMyMetaData : public ISlateMetaData
{
public:
    // 固定写法
    SLATE_METADATA_TYPE(FMyMetaData, ISlateMetaData)

    // 创建函数
    static TSharedRef<FMyMetaData> Create(const FString& InInfo)
    {
        return MakeShared<FMyMetaData>(InInfo);
    }

    // 你想存的数据
    FString CustomInfo;

private:
    FMyMetaData(const FString& InInfo)
        : CustomInfo(InInfo)
    {}
};

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
            ].AddMetaData(FMyMetaData::Create(TEXT("MyCustomData")))
        ];
    }
};


//     SLATE_BEGIN_ARGS(SMyWidget)
//         :  _FitInWindow(true)
//     {}
// 		SLATE_ARGUMENT(bool, FitInWindow)
// 	SLATE_END_ARGS()

// Slate 展开后
// struct FArguments : public TSlateBaseNamedArgs<SMyPanel> 
// { 
//     typedef FArguments WidgetArgsType; 
//     FORCENOINLINE FArguments(): _FitInWindow(true){}
//
//     bool _FitInWindow;		
//     WidgetArgsType& FitInWindow( bool InArg )
//     {
//         _FitInWindow = InArg;
//         return this->Me();
//     }
// }

// FSlateBaseNamedArgs
// 只是存储数据而已
// TSlateBaseNamedArgs
// ✔ 给所有控件的 FArguments 自动生成通用的链式调用方法
// ✔ 让子类 FArguments 支持 .IsEnabled(true) .Visibility() 这种写法
// ✔ 提供安全的链式返回 Me()
// ✔ 提供 AddMetaData 通用功能





// template<class DeclarationType>
// struct NamedSlotProperty
// {
// 	NamedSlotProperty( DeclarationType& InOwnerDeclaration, TAlwaysValidWidget& ContentToSet )
// 		: OwnerDeclaration( InOwnerDeclaration )
// 		, SlotContent(ContentToSet)
// 	{}
//
// 	DeclarationType & operator[]( const TSharedRef<SWidget>& InChild )
// 	{
// 		SlotContent.Widget = InChild;
// 		return OwnerDeclaration;
// 	}
//
// 	DeclarationType & OwnerDeclaration;
// 	TAlwaysValidWidget & SlotContent;
// };


/** A widget reference that is always a valid pointer; defaults to SNullWidget */
// struct TAlwaysValidWidget
// {
// 	TAlwaysValidWidget()
// 	: Widget(SNullWidget::NullWidget)
// 	{
// 	}
//
// 	TSharedRef<SWidget> Widget;
// };
// 所以Content 不需要默认值，因为本身就有一个NullWidget



// SLATE_DEFAULT_SLOT(FArguments, Content)
// 展开后
// NamedSlotProperty<FArguments> Content()
// {
// 	return NamedSlotProperty<FArguments>(*this, _Content);
// }
//
// TAlwaysValidWidget _Content;
// 返回了一个 NamedSlotProperty 的东西，另外他又重载了[]操作符
// 所以支持 .Content()  [ 子控件 ] 这样的语法。 
// 子控件 会被当作 InChild, 将InChild设置成 _Content.Widget



// SNew(SMyPanel)
// [
//     SNew(STextBlock)
// ];
// 触发了下面这个
// FArguments& operator[](const TSharedRef<SWidget> InChild)
// {
// 	_Content.Widget = InChild;
// 	return *this;
// }


// SNew(STextBlock)
//     .Text(TEXT("Hello"))
//     .IsEnabled(true)

// 所以这里才能使用FArguments 的默认参数
// MakeTDecl<STextBlock>(...)
// <<= STextBlock::FArguments()
//     .Text(TEXT("Hello"))
//     .IsEnabled(true);

// SNew(SPanel)[
//     SNew(STextBlock)
// ]
// MakeTDecl<SPanel>(...)
// <<= SPanel::FArguments()[
//    SNew(STextBlock)
// ]