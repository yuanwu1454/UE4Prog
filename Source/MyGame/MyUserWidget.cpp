// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

#include "MyAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"
#include "MyBlueprintFunctinLibrary.h"
#include "MyGameMode.h"
#include "MyObject.h"
#include "Components/TextBlock.h"
#include "Test/PropertyTest.h"
#include "UOBject/Class.h"
#include "UObject/EnumProperty.h"
#include "UObject/Field.h"
#include "Test/FieldToolNS.h"


// 新增：解析参数标志（输入/输出/返回值）
FString GetParamFlagsString(FProperty* Param)
{
    TArray<FString> FlagList;

    if (Param->HasAnyPropertyFlags(CPF_ReturnParm)) FlagList.Add(TEXT("返回值"));
    if (Param->HasAnyPropertyFlags(CPF_OutParm))    FlagList.Add(TEXT("输出"));
    if (Param->HasAnyPropertyFlags(CPF_ReferenceParm)) FlagList.Add(TEXT("引用"));
    // 若没有特殊标志，默认是输入参数
    if (FlagList.Num() == 0 && Param->HasAnyPropertyFlags(CPF_Parm)) FlagList.Add(TEXT("输入"));

    return FString::Join(FlagList, TEXT("/"));
}

// 新增：打印单个函数的所有参数
void PrintFunctionParams(UFunction* Func)
{
    if (!Func) return;

    // 遍历函数的所有属性，筛选出参数
    for (TFieldIterator<FProperty> ParamIt(Func); ParamIt; ++ParamIt)
    {
        FProperty* Param = *ParamIt;
        if (!Param || !Param->HasAnyPropertyFlags(CPF_Parm)) continue; // 只处理参数

        FString ParamName = Param->GetName();
        FString ParamType = Param->GetCPPType();
        FString ParamFlags = GetParamFlagsString(Param);

        // 打印参数详情（缩进2个空格，和函数名区分）
        UE_LOG(LogTemp, Log, TEXT("  └─ 参数名：%-15s | 类型：%-10s | 类型：%s"), 
               *ParamName, *ParamType, *ParamFlags);
    }
}

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

    // 可选绑定，可能为空
    if (Text_Optional)
    {
        Text_Optional->SetText(FText::FromString(TEXT("可选绑定成功")));
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
    // 测试绑定的委托
    if (OnGetCustomText.IsBound())
    {
        FString Result = OnGetCustomText.Execute(100);
        UE_LOG(LogTemp, Log, TEXT("绑定返回：%s"), *Result);
    }


    {
        
        // UMyBlueprintFunctinLibrary::Test(GetWorld());

        // PrintAllWidgetVariableNames();
        // FieldToolNS::CheckClassType(this->GetClass());
        // 步骤1：普通UObject实例（Actor）的GetClass()
        // FieldToolNS::PrintClassInfo(StaticClass());
    }
    

    // UMyObject::Test();
    // UMyAsset::TestCustomMapSerialization();
    // StaticClass();

    // AMyGameMode* GM = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
    // if (GM&&IsValid(GM))
    // {
    //     GM->FindGameSessions();
    // }
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
        FString PropType = FieldToolNS::GetPropertyTypeName(Prop);
        if (PropName == "TestYWMyStructBP")
        {
            PropType = FieldToolNS::GetPropertyTypeName(Prop);
        }

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


    UE_LOG(LogTemp, Log, TEXT("========== 遍历【函数/方法】=========="));
    // 关键修改：遍历 UFunction（函数）而非 FProperty（属性）
    for (TFieldIterator<UFunction> FuncIt(WidgetClass); FuncIt; ++FuncIt)
    {
        UFunction* Func = *FuncIt;
        if (!Func) continue;
        
        // 函数基础信息
        FString FuncName = Func->GetName();
        
        // 获取返回值类型（UE4.27兼容）
        FString ReturnType = FieldToolNS::GetBlueprintFunctionReturnType(Func);
        // 解析函数标志（核心修改：不用StaticEnum）
        FString FuncFlags = FieldToolNS::GetFunctionFlagsString(Func->FunctionFlags);

        UE_LOG(LogTemp, Log, TEXT("函数名：%-20s | 返回值类型：%-10s | 函数标志：%s"), 
               *FuncName, *ReturnType, *FuncFlags);

        PrintFunctionParams(Func);
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
