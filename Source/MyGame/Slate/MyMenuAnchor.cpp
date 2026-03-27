#include "MyMenuAnchor.h"
#include "Logging/LogMacros.h"

void SMyMenuAnchor::Construct(const FArguments& InArgs)
{

    // 遍历查找你自己的类型
    // 获取控件的元数据
    const TArray<TSharedRef<ISlateMetaData>>& MetaDataArr = InArgs.MetaData;
    for (const auto& MD : MetaDataArr)
    {
        if (MD->IsOfType<FMyMetaData>())
        {
            TSharedRef<FMyMetaData> MyMD = StaticCastSharedRef<FMyMetaData>(MD);
            UE_LOG(LogTemp, Log, TEXT("拿到元数据：%s"), *MyMD->CustomInfo);
            
        }
    }
    
    // 调用父类构造，完全继承原生参数
    SMenuAnchor::Construct(
        SMenuAnchor::FArguments()
        .Content()[InArgs._Content.Widget]
        .Padding(InArgs._Padding)
        .MenuContent(InArgs._MenuContent)
        .OnGetMenuContent(InArgs._OnGetMenuContent)
        .OnMenuOpenChanged(InArgs._OnMenuOpenChanged)
        .Placement(InArgs._Placement)
        .FitInWindow(InArgs._FitInWindow)
        .Method(InArgs._Method)
        .ShouldDeferPaintingAfterWindowContent(InArgs._ShouldDeferPaintingAfterWindowContent)
        .UseApplicationMenuStack(InArgs._UseApplicationMenuStack)
        .IsCollapsedByParent(InArgs._IsCollapsedByParent)
        .ApplyWidgetStyleToMenu(InArgs._ApplyWidgetStyleToMenu)
    );
}

void SMyMenuAnchor::SetIsOpen(bool InIsOpen, const bool bFocusMenu, const int32 FocusUserIndex)
{
    // 自定义打开/关闭前逻辑
    if (InIsOpen)
    {
        UE_LOG(LogTemp, Log, TEXT("SMyMenuAnchor：菜单正在打开！"));
        bIsMyMenuOpening = true;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("SMyMenuAnchor：菜单正在关闭！"));
        bIsMyMenuOpening = false;
    }

    // 必须调用父类，否则菜单无法正常工作
    SMenuAnchor::SetIsOpen(InIsOpen, bFocusMenu, FocusUserIndex);
    // 1. 使用菜单栈 + 创建新窗口
    // bUseApplicationMenuStack = true
    // EPopupMethod::CreateNewWindow
    // 最常见：编辑器菜单、右键菜单
    // 独立窗口弹出，由 Slate 全局菜单栈管理
    // 2. 使用菜单栈 + 复用当前窗口
    // bUseApplicationMenuStack = true
    // EPopupMethod::UseCurrentWindow
    // 不创建窗口，直接作为子控件嵌入
    // 适合游戏内 UI、层级内弹窗
    // 3. 不使用菜单栈 + 创建新窗口
    // bUseApplicationMenuStack = false
    // EPopupMethod::CreateNewWindow
    // 自己创建 SWindow，自己管理生命周期
    // 完全自定义弹窗
    // 4. 不使用菜单栈 + 复用当前窗口
    // bUseApplicationMenuStack = false
    // EPopupMethod::UseCurrentWindow
    // 嵌入当前窗口，但不进全局菜单栈
    // 轻量、自定义、独立生命周期
}

void SMyMenuAnchor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SMenuAnchor::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    // 自定义每帧逻辑
    if (bIsMyMenuOpening && IsOpen())
    {
    }
}