#include "FieldToolNS.h"
// 必须添加的头文件（UE4.27 路径）
// 若用到 UBlueprint，还需添加：
// 基础头文件（确保已有）
#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Blueprint/BlueprintSupport.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Engine.h"
#include "UObject/UObjectBaseUtility.h"
#include "UOBject/Class.h"
#include "UObject/EnumProperty.h"
#include "UObject/Field.h"
namespace FieldToolNS
{
	
// 核心函数：获取任意FProperty的类型名称（适配所有类型）
FString GetPropertyTypeName(FProperty* Prop)
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

FString GetFunctionFlagsString(EFunctionFlags Flags)
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

FString GetBlueprintFunctionReturnType(UFunction* Func)
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


    
    void CheckClassType(UClass* InClass)
    {
        if (!InClass) return;

        if(InClass->IsAsset())
        {
            
        }

        // if (InClass->IsA<UBlueprintGeneratedClass>())
        // {
        //     
        // }

        
        
        // 1. 判断是否是蓝图生成的类（核心API）
        // if (InClass->IsA<UBlueprintGeneratedClass>())
        // {
        //     // UE_LOG(LogTemp, Log, TEXT("这是蓝图生成的类：%s"), *InClass->GetName());
        //     //
        //     // // 转为UBPGC，获取蓝图特有信息
        //     // UBlueprintGeneratedClass* BPGClass = Cast<UBlueprintGeneratedClass>(InClass);
        //     // if (BPGClass)
        //     // {
        //     //     // 获取蓝图的原始蓝图资源（.uasset文件）
        //     //     // 步骤2：找到 "Blueprint" 这个成员变量（UObjectProperty 类型）
        //     //     UObjectProperty* BlueprintProp = Cast<UObjectProperty>(
        //     //         BPGClass->FindPropertyByName(TEXT("Blueprint"))
        //     //     );
        //     //     
        //     //     if (BlueprintProp)
        //     //     {
        //     //         // 步骤3：读取该变量的值（即关联的 UBlueprint）
        //     //         UBlueprint* Blueprint = Cast<UBlueprint>(
        //     //             BlueprintProp->GetObjectPropertyValue_InContainer(BPGClass)
        //     //         );
        //     //
        //     //         UE_LOG(LogTemp, Log, TEXT("对应的蓝图资源名：%s"), *Blueprint->GetName());
        //     //         UE_LOG(LogTemp, Log, TEXT("蓝图的父类（C++类）：%s"), *Blueprint->ParentClass->GetName());
        //     //     }
        //     //     else
        //     //     {
        //     //         UE_LOG(LogTemp, Error, TEXT("无法找到UBPGC的Blueprint成员变量！"));
        //     //     }
        //     // }
        // }
        // else
        // {
        //     // UE_LOG(LogTemp, Log, TEXT("这是纯C++类：%s"), *InClass->GetName());
        //     // // 纯C++类的判断：IsNative() 为 true
        //     // if (InClass->IsNative())
        //     // {
        //     //     UE_LOG(LogTemp, Log, TEXT("确认是原生C++类"));
        //     // }
        // }
    }
// 打印UClass的详细信息（包含类基本信息、属性、函数、蓝图关联信息等）
void PrintClassInfo(UClass* InClass)
{
    
    if (!InClass)
    {
        UE_LOG(LogTemp, Error, TEXT("PrintClassInfo: 传入的UClass为空！"));
        return;
    }

    // ======================================
    // 1. 打印类的基础信息
    // ======================================
    UE_LOG(LogTemp, Log, TEXT("======================================"));
    UE_LOG(LogTemp, Log, TEXT("【UClass 详细信息】"));
    UE_LOG(LogTemp, Log, TEXT("======================================"));
    
    // 类名/路径名
    UE_LOG(LogTemp, Log, TEXT("1. 基础信息："));
    UE_LOG(LogTemp, Log, TEXT("   类名：%s"), *InClass->GetName());
    UE_LOG(LogTemp, Log, TEXT("   完整路径名：%s"), *InClass->GetPathName());
    UE_LOG(LogTemp, Log, TEXT("   类的显示名：%s"), *InClass->GetDisplayNameText().ToString());
    
    // 父类信息
    UClass* SuperClass = InClass->GetSuperClass();
    if (SuperClass)
    {
        UE_LOG(LogTemp, Log, TEXT("   父类名：%s"), *SuperClass->GetName());
        UE_LOG(LogTemp, Log, TEXT("   父类完整路径：%s"), *SuperClass->GetPathName());
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("   父类：无（根类）"));
    }

    // 类类型判断（原生C++类/蓝图类）
    UE_LOG(LogTemp, Log, TEXT("   是否为原生C++类：%s"), InClass->IsNative() ? TEXT("是") : TEXT("否"));
    // UE_LOG(LogTemp, Log, TEXT("   是否为蓝图生成类：%s"), InClass->IsA<UBlueprintGeneratedClass>() ? TEXT("是") : TEXT("否"));
    UE_LOG(LogTemp, Log, TEXT("   是否为抽象类：%s"), InClass->HasAnyClassFlags(CLASS_Abstract) ? TEXT("是") : TEXT("否"));
    // UE_LOG(LogTemp, Log, TEXT("   是否为蓝图可见：%s"), InClass->HasAnyClassFlags(CLASS_BlueprintVisible) ? TEXT("是") : TEXT("否"));

    // ======================================
    // 2. 打印蓝图类特有信息（如果是UBPGC）
    // ======================================
    if (UBlueprintGeneratedClass* BPGClass2 = Cast<UBlueprintGeneratedClass>(InClass))
    {
        UE_LOG(LogTemp, Log, TEXT("2. 蓝图类特有信息："));
        
        // UE4.27 获取关联的蓝图资源（反射方式）
        UClass* BPGCClass = BPGClass2->GetClass();
        UObjectProperty* BlueprintProp2 = Cast<UObjectProperty>(BPGCClass->FindPropertyByName(TEXT("Blueprint")));
        if (BlueprintProp2)
        {
            UBlueprint* Blueprint = Cast<UBlueprint>(BlueprintProp2->GetObjectPropertyValue_InContainer(BPGClass2));
            if (Blueprint)
            {
                UE_LOG(LogTemp, Log, TEXT("   关联蓝图名：%s"), *Blueprint->GetName());
                UE_LOG(LogTemp, Log, TEXT("   蓝图路径：%s"), *Blueprint->GetPathName());
                // UE_LOG(LogTemp, Log, TEXT("   蓝图是否已编译：%s"), Blueprint->IsCompiled() ? TEXT("是") : TEXT("否"));
                UE_LOG(LogTemp, Log, TEXT("   蓝图父类（C++）：%s"), *Blueprint->ParentClass->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("   关联蓝图：无"));
            }
        }
    }
}
};
