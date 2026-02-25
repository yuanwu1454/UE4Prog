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
    UE_LOG(LogTemp, Log, TEXT("======== 开始打印UserWidget属性信息 ========"));
    if (!WidgetTree)
    {
        UE_LOG(LogTemp, Warning, TEXT("WidgetTree为空，无法获取控件列表！"));
        return;
    }

    // ========== 第一步：打印所有UPROPERTY修饰的属性（含类型） ==========
    UE_LOG(LogTemp, Log, TEXT("\n【1. 所有UPROPERTY属性（变量名+类型）】"));
    UClass* WidgetClass = this->GetClass();
    for (TFieldIterator<FProperty> PropIt(WidgetClass); PropIt; ++PropIt)
    {
        FProperty* Prop = *PropIt;
        if (!Prop) continue;

        // 获取属性名称 + 类型名称
        FString PropName = Prop->GetName();
        FString PropType = GetPropertyTypeName(Prop);

        UE_LOG(LogTemp, Log, TEXT("变量名：%-20s | 类型：%s"), *PropName, *PropType);
    }

    // ========== 第二步：打印所有控件（实例名+绑定变量名+类型） ==========
    UE_LOG(LogTemp, Log, TEXT("\n【2. 所有控件（实例名+绑定变量名+控件类型）】"));
    TArray<UWidget*> AllWidgets;
    WidgetTree->GetAllWidgets(AllWidgets);
    for (UWidget* Widget : AllWidgets)
    {
        if (!Widget) continue;

        FString WidgetInstanceName = Widget->GetName(); // 控件实例名
        FString BoundVarName = GetWidgetVariableName(Widget); // 绑定的变量名
        FString WidgetType = Widget->GetClass()->GetName(); // 控件类型

        UE_LOG(LogTemp, Log, TEXT("实例名：%-20s | 绑定变量名：%-20s | 控件类型：%s"),
            *WidgetInstanceName, *BoundVarName, *WidgetType);
    }

    UE_LOG(LogTemp, Log, TEXT("\n======== 打印结束 ========"));
}

FString UMyUserWidget::GetWidgetVariableName(UWidget* Widget)
{
    if (!Widget || !this) return TEXT("无绑定");

    UClass* WidgetClass = this->GetClass();
    if (!WidgetClass) return TEXT("无绑定");

    for (TFieldIterator<FProperty> PropIt(WidgetClass); PropIt; ++PropIt)
    {
        FProperty* Prop = *PropIt;
        if (!Prop || !Prop->IsA<FObjectProperty>()) continue;

        FObjectProperty* ObjectProp = CastField<FObjectProperty>(Prop);
        if (!ObjectProp || !ObjectProp->PropertyClass->IsChildOf(UWidget::StaticClass())) continue;

        // 整个类就是一个笔记本
        // Prop 实际上就是一个一个的目录 记录了【变量名 变量在笔记本的位置】
        // Prop->ContainerPtrToValuePtr<void>(this)
        // 表示根据目录找到 变量的物理位置
        // GetObjectPropertyValue：根据变量的物理位置 找到 读取变量 “页面” 上的内容
        UObject* PropValue = ObjectProp->GetObjectPropertyValue(Prop->ContainerPtrToValuePtr<void>(this));
        if (PropValue == Widget)
        {
            return Prop->GetName();
        }
    }

    return TEXT("无绑定");
}

void UMyUserWidget::PrintAllProp()
{
    // 反射获取当前UserWidget的类信息
    UClass* WidgetClass = this->GetClass();
    if (!WidgetClass) return ;
    // 关键修改1：遍历FProperty（替代UProperty）
    for (TFieldIterator<FProperty> PropIt(WidgetClass); PropIt; ++PropIt)
    {
        FProperty* Prop = *PropIt;
        if (!Prop) continue;

        FString PropName = Prop->GetName();
        FString PropType = GetPropertyTypeName(Prop);
        UE_LOG(LogTemp, Log, TEXT("变量名：%-20s | 类型：%s"), *PropName, *PropType);
    }
}

// 核心函数：获取任意FProperty的类型名称（适配所有类型）
FString UMyUserWidget::GetPropertyTypeName(FProperty* Prop)
{
    if (!Prop) return TEXT("Unknown");

    // 1. 基础类型（int/float/bool/FString等）
    FString BasicType = Prop->GetCPPType(false); // false=不包含命名空间，更简洁
    if (!BasicType.IsEmpty())
    {
        // 对FString做特殊优化（GetCPPType返回的是FString::CStrRef，优化为FString）
        if (BasicType.Contains(TEXT("FString")))
        {
            return TEXT("FString");
        }
        return BasicType;
    }

    // 2. 对象类型（UObject子类：UButton/UTextBlock/Actor等）
    if (FObjectProperty* ObjectProp = CastField<FObjectProperty>(Prop))
    {
        if (ObjectProp->PropertyClass)
        {
            return ObjectProp->PropertyClass->GetName(); // 返回具体类名（如UButton）
        }
        return TEXT("UObject");
    }

    // 3. 数组类型（如TArray<int32>、TArray<UButton*>）
    if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Prop))
    {
        FString InnerType = GetPropertyTypeName(ArrayProp->Inner);
        return FString::Printf(TEXT("TArray<%s>"), *InnerType);
    }

    // 4. 结构体类型（如FVector、FRotator）
    if (FStructProperty* StructProp = CastField<FStructProperty>(Prop))
    {
        if (StructProp->Struct)
        {
            return StructProp->Struct->GetName();
        }
        return TEXT("UStruct");
    }

    // 5. 其他类型（枚举、委托等）
    return Prop->GetClass()->GetName();
}