#include "MyMenuAnchor.h"
#include "Logging/LogMacros.h"

void SMyMenuAnchor::Construct(const FArguments& InArgs)
{

    // 遍历查找你自己的类型
    // 获取控件的元数据
    const TArray<TSharedRef<ISlateMetaData>>& MetaData = InArgs.MetaData;
    for (const auto& MD : MetaData)
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
}

void SMyMenuAnchor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SMenuAnchor::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    // 自定义每帧逻辑
    if (bIsMyMenuOpening && IsOpen())
    {
    }
}