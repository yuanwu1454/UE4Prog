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
    FMyMetaData(const FString& InInfo)
        : CustomInfo(InInfo)
    {}
};

/**
 * 自定义 MenuAnchor，完全继承引擎原生 SMenuAnchor
 * 可重写：打开/关闭、菜单位置、菜单逻辑
 */
class MYGAME_API SMyMenuAnchor : public SMenuAnchor
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



// 👇 终极完整版：包含 TAttribute 所有用法案例
class SMyMenuAnchorTestWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMyMenuAnchorTestWidget) {}
	SLATE_END_ARGS()

	TSharedPtr<SMyMenuAnchor> MyMenuAnchor;

	// 动态数据
	float CurrentHealth = 1.0f;
	int ClickCount = 0;
	FString CustomTipText = TEXT("初始提示");

	// 用于 BindSP 测试
	TSharedPtr<FString> SPStringObject;

public:
	void Construct(const FArguments& InArgs)
	{
		// 初始化共享指针对象（用于 BindSP 演示）
		SPStringObject = MakeShared<FString>(TEXT("SP指针提供的文字"));

		ChildSlot
		[
			SAssignNew(MyMenuAnchor, SMyMenuAnchor)
			.MenuContent(
				SNew(SBorder)
				// ------------------------------------------------
				// ✅ 1. TAttribute 固定值
				// ------------------------------------------------
				.BorderBackgroundColor(TAttribute<FLinearColor>(FLinearColor::Black))
				.Padding(TAttribute<FMargin>(FMargin(10)))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("菜单内容")))
				]
			)
			[
				SNew(SButton)

				// ==============================================
				// ✅ 2. Slate 内置 _Lambda（自动转 TAttribute）
				// ==============================================
				.Text_Lambda([this]()
				{
					return FText::FromString(FString::Printf(TEXT("点击: %d"), ClickCount));
				})

				// ==============================================
				// ✅ 3. MakeAttributeLambda（最常用）
				// ==============================================
				.ForegroundColor(MakeAttributeLambda([this]()
				{
					return CurrentHealth < 0.5f ? FLinearColor::Red : FLinearColor::Green;
				}))

				// ==============================================
				// ✅ 4. BindUObject（绑定 UObject/类成员函数）
				// ==============================================
				.IsEnabled(TAttribute<bool>::Create(
					TAttribute<bool>::FGetter::CreateUObject(this, &SMyMenuAnchorTestWidget::IsButtonEnabled)
				))

				// ==============================================
				// ✅ 5. BindRaw（裸指针绑定）
				// ==============================================
				.ToolTipText(MakeAttributeRaw<FText>(this, &SMyMenuAnchorTestWidget::GetToolTipTextRaw))
				// ==============================================
				// ✅ 6. BindSP（共享指针 TSharedPtr 绑定）
				// ==============================================
				// .ButtonColorAndOpacity(MakeAttributeSP<FLinearColor>(
				// 	SPStringObject.Get(),
				// 	&FString::Len // 用长度动态控制透明度)
				// 	))
				//
				// // ==============================================
				// // ✅ 7. BindStatic（静态函数绑定）
				// // ==============================================
				// .DesiredSizeScale(MakeAttributeStatic<float>(&GetStaticScale))
				//
				// // ==============================================
				// // ✅ 8. 手动创建 TAttribute（完整写法）
				// // ==============================================
				.ButtonColorAndOpacity_Lambda([this]()
				{
					return ClickCount % 2 == 0 ? FLinearColor::Blue : FLinearColor::Yellow;
				})

				.OnClicked(FOnClicked::CreateLambda([this]()
				{
					if (MyMenuAnchor.IsValid())
						MyMenuAnchor->SetIsOpen(true);

					ClickCount++;
					CurrentHealth = 0.3f;
					CustomTipText = TEXT("已点击！");
					return FReply::Handled();
				}))
			]
			.AddMetaData(FMyMetaData::Create(TEXT("MyCustomData")))
		];
	}

	// ------------------------------
	// 供 BindUObject 使用
	// ------------------------------
	bool IsButtonEnabled() const
	{
		return ClickCount < 10;
	}

	// ------------------------------
	// 供 BindRaw 使用
	// ------------------------------
	FText GetToolTipTextRaw() const
	{
		return FText::FromString(CustomTipText);
	}

	// ------------------------------
	// 静态函数（供 BindStatic 使用）
	// ------------------------------
	static float GetStaticScale()
	{
		return 1.0f; // 静态函数无this，全局逻辑
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
// 所以支持 * .Content()  [ 子控件 ] *
// 这样的语法。 
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


