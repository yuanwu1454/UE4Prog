// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"
#include "MyBlueprintFunctinLibrary.h"
#include "Test/PropertyTest.h"
#include "UOBject/Class.h"
#include "UObject/EnumProperty.h"
#include "UObject/Field.h"


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
        if (PropName == "TestYWMyStructBP")
        {
            PropType = GetPropertyTypeName(Prop);
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
        FString ReturnType = GetBlueprintFunctionReturnType(Func);
        // 解析函数标志（核心修改：不用StaticEnum）
        FString FuncFlags = GetFunctionFlagsString(Func->FunctionFlags);

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

void UMyUserWidget::PrintAllProp()
{
}

// 核心函数：获取任意FProperty的类型名称（适配所有类型）
FString UMyUserWidget::GetPropertyTypeName(FProperty* Prop)
{
    if (!Prop){ return TEXT("UNKNOWN");}

    // 第一步：获取CPP类型字符串
    FString BasicType = Prop->GetCPPType();
    if (!BasicType.IsEmpty())
    {
        // 定义基础类型白名单（只处理这些类型，其余跳过）
        static TSet<FString> BasicTypeWhitelist = {
            TEXT("FString"), TEXT("FText"), TEXT("bool"),
            TEXT("int32"), TEXT("int64"), TEXT("float"),
            TEXT("double"), TEXT("uint8"), TEXT("uint16"),
            TEXT("uint32"), TEXT("uint64"), TEXT("FName"),
            TEXT("FVector"), TEXT("FRotator"), TEXT("FVector2D") // 可根据你的需求补充
        };

        // 先统一格式化（比如把"const FString&"转为"FString"）
        FString CleanType = BasicType;
        // 移除const、引用&、指针*等修饰符
        CleanType.ReplaceInline(TEXT("const "), TEXT(""), ESearchCase::IgnoreCase);
        CleanType.ReplaceInline(TEXT("&"), TEXT(""));
        CleanType.ReplaceInline(TEXT("*"), TEXT(""));
        CleanType = CleanType.TrimStartAndEnd();

        // 只有在白名单内，才返回基础类型
        if (BasicTypeWhitelist.Contains(CleanType))
        {
            return CleanType;
        }
        // 非基础类型（如TArray），跳过基础分支，走后面的逻辑
    }


    // ========== 2. 枚举类型（FEnumProperty） ==========
    FEnumProperty* EnumProp = CastField<FEnumProperty>(Prop);
    if (EnumProp)
    {
        if (EnumProp->GetEnum())
        {
            // 返回枚举的实际名称（如EMyCustomEnum）
            return EnumProp->GetEnum()->GetName();
        }
        return TEXT("Enum");
    }

    
    // ========== 3. 委托类型（FDelegateProperty） ==========
    FDelegateProperty* DelegateProp = CastField<FDelegateProperty>(Prop);
    if (DelegateProp)
    {
        if (DelegateProp->SignatureFunction)
        {
            FString SignatureFunctionName = DelegateProp->SignatureFunction->GetName();
            // UE_LOG(LogTemp, Log, TEXT("Delegte Begin %s "), *SignatureFunctionName);
            // for (TFieldIterator<FProperty> PropIt(DelegateProp->SignatureFunction); PropIt; ++PropIt)
            // {
            //     FProperty* Prop = *PropIt;
            //     if (!Prop) continue;
            //
            //     FString PropName = Prop->GetName();
            //     FString PropType = GetPropertyTypeName(Prop);
            //     UE_LOG(LogTemp, Log, TEXT("变量名：%-20s | 类型：%s"), *PropName, *PropType);
            // }
            // UE_LOG(LogTemp, Log, TEXT("Delegte End %s "), *SignatureFunctionName);
            
            // 委托类型格式：Delegate<函数签名所属类::函数名>
            FString DelegateName = FString::Printf(TEXT("Delegate<%s::%s>"),
                *DelegateProp->SignatureFunction->GetOuter()->GetName(),
                *DelegateProp->SignatureFunction->GetName());
            
            return DelegateName;
        }
        return TEXT("Delegate");
    }

    // Object
    FObjectProperty* ObjectProp = CastField<FObjectProperty>(Prop);
    if(ObjectProp)
    {
        if (ObjectProp->PropertyClass)
        {
            return ObjectProp->PropertyClass->GetName();   
        }
        return TEXT("UObject");
    }

    // Array
    FArrayProperty* ArrayProp = CastField<FArrayProperty>(Prop);
    if(ArrayProp)
    {
        FString InnerType = GetPropertyTypeName(ArrayProp->Inner);
        return FString::Printf(TEXT("TArray<%s>"), *InnerType);
    }

    // Struct
    FStructProperty* StructProperty = CastField<FStructProperty>(Prop);
    if(StructProperty)
    {
        if (StructProperty->Struct)
        {

            // 关键修改1：遍历FProperty（替代UProperty）
            // FString StructName = StructProperty->Struct->GetName();
            // UE_LOG(LogTemp, Log, TEXT("Struct Begin %s "), *StructName);
            // for (TFieldIterator<FProperty> PropIt(StructProperty->Struct); PropIt; ++PropIt)
            // {
            //     FProperty* Prop = *PropIt;
            //     if (!Prop) continue;
            //
            //     FString PropName = Prop->GetName();
            //     FString PropType = GetPropertyTypeName(Prop);
            //     UE_LOG(LogTemp, Log, TEXT("变量名：%-20s | 类型：%s"), *PropName, *PropType);
            // }
            // UE_LOG(LogTemp, Log, TEXT("Struct End %s "), *StructName);
            return StructProperty->Struct->GetName();
        }
        return TEXT("UStruct");
    }

    // ========== 6. Set类型（TSet） ==========
    FSetProperty* SetProp = CastField<FSetProperty>(Prop);
    if (SetProp)
    {
        // 递归获取Set内部元素的类型
        FString InnerType = GetPropertyTypeName(SetProp->ElementProp);
        return FString::Printf(TEXT("TSet<%s>"), *InnerType);
    }
    
    // ========== 7. Map类型（TMap） ==========
    FMapProperty* MapProp = CastField<FMapProperty>(Prop);
    if (MapProp)
    {
        // 分别获取Key和Value的类型，递归调用
        FString KeyType = GetPropertyTypeName(MapProp->KeyProp);
        FString ValueType = GetPropertyTypeName(MapProp->ValueProp);
        return FString::Printf(TEXT("TMap<%s, %s>"), *KeyType, *ValueType);
    }
    
    return Prop->GetClass()->GetName();

}

FString UMyUserWidget::GetFunctionFlagsString(EFunctionFlags Flags)
{
    TArray<FString> FlagList;

    // 解析常用的函数标志（按需添加）
    if (Flags & FUNC_Public)            FlagList.Add(TEXT("Public"));
    if (Flags & FUNC_Private)           FlagList.Add(TEXT("Private"));
    if (Flags & FUNC_Protected)         FlagList.Add(TEXT("Protected"));
    if (Flags & FUNC_BlueprintCallable) FlagList.Add(TEXT("BlueprintCallable"));
    if (Flags & FUNC_BlueprintPure)     FlagList.Add(TEXT("BlueprintPure"));
    if (Flags & FUNC_Native)            FlagList.Add(TEXT("Native"));
    if (Flags & FUNC_Event)             FlagList.Add(TEXT("Event"));
    if (Flags & FUNC_BlueprintEvent)    FlagList.Add(TEXT("BlueprintEvent"));

    // 拼接成字符串
    return FString::Join(FlagList, TEXT(", "));
}

FString UMyUserWidget::GetBlueprintFunctionReturnType(UFunction* Func)
{
    if (!Func) return TEXT("void");

    // 第一步：先尝试获取C++函数的返回值（兼容原有逻辑）
    FProperty* ReturnProp = Func->GetReturnProperty();
    if (ReturnProp)
    {
        return ReturnProp->GetCPPType();
    }

    // 第二步：遍历参数，找蓝图函数的返回值（核心修正）
    for (TFieldIterator<FProperty> ParamIt(Func); ParamIt; ++ParamIt)
    {
        FProperty* Param = *ParamIt;
        if (!Param) continue;

        // 蓝图返回值的参数会被标记为「ReturnParm + Parm」
        if (Param->HasAnyPropertyFlags(CPF_ReturnParm | CPF_Parm))
        {
            return Param->GetCPPType();
        }
    }

    return TEXT("void");
}

// UStruct 是 UE 反射系统中 “结构化类型” 的基类，范围极广（包含类、结构体、函数等）；
// UScriptStruct 是 UStruct 的子类，仅对应蓝图 / C++ 中的 “纯结构体”（USTRUCT () 标记的 FStruct）；
// 在处理结构体属性（FStructProperty）时，实际要操作的是 UScriptStruct，而非基类 UStruct。
// 1. 判断一个UStruct* 是否是“结构体”（UScriptStruct）
// UStruct* SomeStruct = ...;
// if (SomeStruct->IsA<UScriptStruct>())
// {
//     // 这是一个可实例化的USTRUCT()结构体（如FVector）
//     UScriptStruct* ScriptStruct = Cast<UScriptStruct>(SomeStruct);
// }
// else if (SomeStruct->IsA<UClass>())
// {
//     // 这是一个UE类（如UWidget、AActor）
//     UClass* Class = Cast<UClass>(SomeStruct);
// }
// else if (SomeStruct->IsA<UFunction>())
// {
//     // 这是一个函数
//     UFunction* Func = Cast<UFunction>(SomeStruct);
// }