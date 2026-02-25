// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"
#include "MyBlueprintFunctinLibrary.h"

// ========== 重写Widget构造函数（打开UI时） ==========
void UMyUserWidget::NativeConstruct()
{
    // 先调用父类的NativeConstruct，保证Widget正常初始化
    Super::NativeConstruct();

    // 打开UI时，切换输入模式
    SwitchToUIMode();


    // 安全检查：确保buttonA成功绑定（避免空指针崩溃）
    if (Button_A)
    {
        // 绑定点击事件：将按钮的OnClicked委托绑定到自定义的回调函数
        Button_A->OnClicked.AddUniqueDynamic(this, &UMyUserWidget::OnButtonAClicked);
    }
    else
    {
        // 调试提示：如果绑定失败，检查蓝图中Button的名字是否为buttonA
        UE_LOG(LogTemp, Warning, TEXT("UMyCustomWidget: buttonA绑定失败！请检查蓝图中Button的名字是否为buttonA"));
    }
}

// ========== 重写Widget销毁函数（关闭UI时） ==========
void UMyUserWidget::NativeDestruct()
{
    // 关闭UI时，恢复游戏输入模式
    RestoreToGameMode();

    // 后调用父类的NativeDestruct，保证Widget正常销毁
    Super::NativeDestruct();
}

// ========== 切换到UI输入模式 ==========
void UMyUserWidget::SwitchToUIMode()
{
    // 安全获取玩家控制器
    APlayerController* PC = GetSafePlayerController();
    if (!PC) return;

    // ========== 核心：设置输入模式 ==========
    // 方案1：允许同时操作「游戏」和「UI」（推荐，如边移动边操作背包）
    FInputModeGameAndUI InputModeGameAndUI;
    // 绑定当前UI为「焦点UI」，确保UI能接收输入（可选，提升UI交互稳定性）
    InputModeGameAndUI.SetWidgetToFocus(this->TakeWidget());
    // 禁止鼠标锁定到视口（可选，根据需求设置）
    InputModeGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputModeGameAndUI);

    // 方案2：仅允许操作「UI」（如暂停菜单、剧情对话，禁止玩家移动）
    // FInputModeUIOnly InputModeUIOnly;
    // InputModeUIOnly.SetWidgetToFocus(this->TakeWidget());
    // InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    // PC->SetInputMode(InputModeUIOnly);

    // 显示鼠标光标（UI操作必备）
    PC->bShowMouseCursor = true;
}

// ========== 恢复到游戏输入模式 ==========
void UMyUserWidget::RestoreToGameMode()
{
    // 安全获取玩家控制器
    APlayerController* PC = GetSafePlayerController();
    if (!PC) return;

    // ========== 核心：恢复仅游戏输入 ==========
    FInputModeGameOnly InputModeGameOnly;
    PC->SetInputMode(InputModeGameOnly);

    // 隐藏鼠标光标
    PC->bShowMouseCursor = false;
}

// ========== 安全获取玩家控制器（避坑关键） ==========
APlayerController* UMyUserWidget::GetSafePlayerController()
{
    // 方式1：通过GameplayStatics获取第一个玩家控制器（通用，推荐）
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    // 方式2：若Widget有持有PlayerController的引用，可直接返回（适合多玩家场景）
    // if (MyCachedPlayerController) return MyCachedPlayerController;

    return PC;
}

UWorld* UMyUserWidget::GetSelfWorld()
{
    return GetWorld();
}

void UMyUserWidget::OnButtonAClicked()
{
    // 这里写按钮点击后的逻辑，比如打印日志、触发其他功能等
    UE_LOG(LogTemp, Log, TEXT("按钮buttonA被点击了！"));
    
    // UMyBlueprintFunctinLibrary::Test(GetWorld());

    PrintAllWidgetVariableNames();
    
}

void UMyUserWidget::PrintAllWidgetVariableNames()
{
    UE_LOG(LogTemp, Log, TEXT("======== 开始打印UserWidget控件变量名 ========"));

    if (!WidgetTree)
    {
        UE_LOG(LogTemp, Warning, TEXT("WidgetTree为空，无法获取控件列表！"));
        return;
    }

    // 2. 遍历WidgetTree中的所有控件
    TArray<UWidget*> AllWidgets;

    WidgetTree->GetAllWidgets(AllWidgets);
    for (UWidget* Widget : AllWidgets)
    {
        if (!Widget) continue;

        // 3. 获取控件的变量名（蓝图/C++绑定的名字）
        FString VariableName = GetWidgetVariableName(Widget);
        // 4. 获取控件的类型名（比如UButton、UTextBlock）
        FString WidgetTypeName = Widget->GetClass()->GetName();

        // 5. 打印结果：变量名 + 控件类型
        if (!VariableName.IsEmpty())
        {
            UE_LOG(LogTemp, Log, TEXT("控件变量名：%s | 控件类型：%s"), *VariableName, *WidgetTypeName);
        }
        else
        {
            // 无变量名的控件（未勾选Is Variable，也未被C++绑定）
            UE_LOG(LogTemp, Log, TEXT("无变量名控件 | 控件类型：%s | 控件实例名：%s"), 
                *WidgetTypeName, *Widget->GetName());
        }
    }

    UE_LOG(LogTemp, Log, TEXT("======== 打印结束 ========"));
}

FString UMyUserWidget::GetWidgetVariableName(UWidget* Widget)
{
    if (!Widget || !this) return TEXT("");

    // 反射获取当前UserWidget的类信息
    UClass* WidgetClass = this->GetClass();
    if (!WidgetClass) return TEXT("");

    // 遍历UserWidget的所有属性（变量）
    for (TFieldIterator<UProperty> PropIt(WidgetClass); PropIt; ++PropIt)
    {
        UProperty* Prop = *PropIt;
        if (!Prop) continue;

        // 过滤：只找指向UWidget的属性（控件变量）
        if (Prop->IsA<UObjectProperty>())
        {
            UObjectProperty* ObjectProp = Cast<UObjectProperty>(Prop);
            // 检查属性类型是否为UWidget子类（Button、TextBlock等）
            if (ObjectProp && ObjectProp->PropertyClass->IsChildOf(UWidget::StaticClass()))
            {
                // 获取该属性的值（即控件指针）
                UObject* PropValue = ObjectProp->GetObjectPropertyValue(Prop->ContainerPtrToValuePtr<void>(this));
                // 如果属性值等于当前遍历的Widget，说明找到对应的变量名
                if (PropValue == Widget)
                {
                    return Prop->GetName(); // 返回变量名（比如buttonA）
                }
            }
        }
    }

    // 如果没找到C++绑定的变量名，检查蓝图中是否有命名（Is Variable）
    FString WidgetName = Widget->GetName();
    // 蓝图中勾选Is Variable的控件，名字会和变量名一致，这里直接返回
    return WidgetName.IsEmpty() ? TEXT("") : WidgetName;
}