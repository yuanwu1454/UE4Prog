# UE 宏标记（UPROPERTY/UFUNCTION）纳入 Field 成员 伪代码讲解

核心逻辑：UE 的 `UPROPERTY()`、`UFUNCTION()` 不是普通宏，是「反射注册宏」—— 本质是通过宏定义，让 UE 编译工具（UnrealHeaderTool，简称 UHT）在编译时，自动生成反射代码，把标记的成员封装成对应的 FField 子类（FProperty/UFunction），再挂载到类对象的 Field 列表中，最终实现遍历。

全程分 3 步：1. 开发者写带宏的代码 → 2. UHT 自动生成反射代码 → 3. 类对象初始化时加载 Field 成员，供 TFieldIterator 遍历。下面用伪代码完整模拟。

## 第一步：开发者编写带宏标记的原始代码（你写的代码）

先定义一个 UObject 子类，用 UPROPERTY/UFUNCTION 标记需要纳入反射的成员（变量/函数）。

```cpp
// 1. 基础宏：声明类是反射类（必须有 GENERATED_BODY()）
UCLASS() // 标记这是一个可反射的 UObject 子类
class UMyTestClass : public UObject
{
    GENERATED_BODY() // 关键：留给 UHT 自动生成反射代码的「占位符」

public:
    // 2. UPROPERTY() 标记变量：要纳入反射，生成 FProperty
    UPROPERTY()
    int32 MyReflectInt; // 要被封装成 FIntProperty（FField 子类）

    // 3. 无 UPROPERTY() 标记：不纳入反射
    int32 MyNormalInt; // 无对应的 FProperty，反射系统看不到

public:
    // 4. UFUNCTION() 标记函数：要纳入反射，生成 UFunction
    UFUNCTION()
    void MyReflectFunc(int32 Param); // 要被封装成 UFunction（FField 子类）

    // 5. 无 UFUNCTION() 标记：不纳入反射
    void MyNormalFunc(); // 无对应的 UFunction，无法被遍历
};

```

## 第二步：UHT 自动生成反射代码（核心步骤，你看不到但真实存在）

UE 编译时，UHT 会扫描你的代码，识别 UCLASS/UPROPERTY/UFUNCTION 宏，自动生成对应的反射代码（通常在 Generated 文件夹下，比如 MyTestClass.generated.h/.cpp）。

下面是 UHT 生成的「反射注册伪代码」，核心是：创建 FField 子类对象，绑定到 UMyTestClass 的类元数据，直接将 Field 成员（FProperty/UFunction）纳入类对象，供后续遍历使用。

重点：UHT 会自动补全 GENERATED_BODY() 对应的实现，核心是「注册 Field 成员」，伪代码如下（完全模拟底层逻辑，可直接对应你写的原始代码）：

```cpp
// UHT 自动生成的反射代码（MyTestClass.generated.cpp 核心内容）
// 1. 生成类的反射元数据，用于存储 Field 列表（类对象的核心数据）
struct UMyTestClass_ClassMeta
{
    // 类的 Field 列表：存储所有反射的 FField 子类（FProperty/UFunction）
    TArray<FField*> ClassFields; // 关键容器：所有反射成员都存在这里

    // 构造函数：初始化时，将 UPROPERTY/UFUNCTION 标记的成员，封装成 Field 并加入列表
    UMyTestClass_ClassMeta()
    {
        // -------------------------- 处理 UPROPERTY() 标记的变量 MyReflectInt --------------------------
        // 1. 创建 FField 子类（FIntProperty），绑定 MyReflectInt 变量
        FIntProperty* MyReflectInt_Prop = new FIntProperty();
        // 2. 给 Field 赋值：绑定变量名、变量类型、所属类、变量内存偏移（关键：让 Field 找到变量）
        MyReflectInt_Prop->SetName(TEXT("MyReflectInt")); // 对应原始变量名
        MyReflectInt_Prop->SetCPPType(TEXT("int32")); // 对应变量类型
        MyReflectInt_Prop->SetOwnerClass(UMyTestClass::StaticClass()); // 绑定到当前类
        // 内存偏移：告诉 Field，这个变量在类对象中的内存位置（UE 底层计算）
        MyReflectInt_Prop->SetOffset(MEMOFFSET(UMyTestClass, MyReflectInt));
        // 3. 将这个 FProperty（FField 子类）加入类的 Field 列表 → 纳入类对象
        ClassFields.Add(MyReflectInt_Prop);

        // -------------------------- 处理 UFUNCTION() 标记的函数 MyReflectFunc --------------------------
        // 1. 创建 FField 子类（UFunction），绑定 MyReflectFunc 函数
        UFunction* MyReflectFunc_Func = new UFunction();
        // 2. 给 Field 赋值：绑定函数名、函数签名、所属类、函数地址
        MyReflectFunc_Func->SetName(TEXT("MyReflectFunc")); // 对应原始函数名
        MyReflectFunc_Func->SetFunctionSignature(TEXT("void(int32)")); // 函数签名（参数+返回值）
        MyReflectFunc_Func->SetOwnerClass(UMyTestClass::StaticClass()); // 绑定到当前类
        MyReflectFunc_Func->SetFunctionPtr(&UMyTestClass::MyReflectFunc); // 绑定函数实际地址
        // 3. 处理函数参数（补充：参数也是 FProperty，同样加入函数的 Field 列表）
        FIntProperty* Param_Prop = new FIntProperty();
        Param_Prop->SetName(TEXT("Param"));
        Param_Prop->SetCPPType(TEXT("int32"));
        Param_Prop->SetPropertyFlags(CPF_Parm); // 标记为函数参数
        MyReflectFunc_Func->AddProperty(Param_Prop); // 函数自己也有 Field 列表（存参数）
        // 4. 将这个 UFunction（FField 子类）加入类的 Field 列表 → 纳入类对象
        ClassFields.Add(MyReflectFunc_Func);

        // -------------------------- 关键：无宏标记的成员，不生成 Field --------------------------
        // MyNormalInt（无 UPROPERTY）、MyNormalFunc（无 UFUNCTION）：无对应的 Field 对象，不加入 ClassFields
        // 所以反射系统看不到，TFieldIterator 也遍历不到
    }
};

// 2. 全局实例化类元数据，让 UMyTestClass 类对象能访问到 Field 列表
static UMyTestClass_ClassMeta UMyTestClass_Meta;

// 3. 给 UMyTestClass 绑定元数据（核心：让类对象「持有」Field 列表）
// 重载 UMyTestClass 的 GetClassFields() 方法，返回生成的 Field 列表
TArray<FField*> UMyTestClass::GetClassFields() const
{
    return UMyTestClass_Meta.ClassFields;
}
```

## 第三步：类对象初始化 + TFieldIterator 遍历（验证宏标记的效果）

当你创建 UMyTestClass 的实例（类对象）时，UE 会自动加载上述反射元数据，让类对象持有 Field 列表。此时 TFieldIterator 就能通过遍历这个列表，找到对应的 Field 成员，伪代码如下：

```cpp
// 模拟你使用 TFieldIterator 遍历的代码
void TestFieldTraversal()
{
    // 1. 创建 UMyTestClass 类对象（初始化时自动加载 Field 列表）
    UMyTestClass* MyClassObj = NewObject<UMyTestClass>();

    // 2. 获取类对象的 Field 列表（就是 UHT 生成的 UMyTestClass_Meta.ClassFields）
    UClass* MyClass = MyClassObj->GetClass();
    TArray<FField*> AllFields = MyClass->GetClassFields();

    // 3. 遍历 FProperty（变量）：只能找到 UPROPERTY() 标记的 MyReflectInt
    UE_LOG(LogTemp, Log, TEXT("=== 遍历反射变量（FProperty） ==="));
    for (TFieldIterator<FProperty> PropIt(MyClass); PropIt; ++PropIt)
    {
        FProperty* Prop = *PropIt;
        UE_LOG(LogTemp, Log, TEXT("反射变量：%s | 类型：%s"), *Prop->GetName(), *Prop->GetCPPType());
        // 输出结果：反射变量：MyReflectInt | 类型：int32（MyNormalInt 无 Field，不显示）
    }

    // 4. 遍历 UFunction（函数）：只能找到 UFUNCTION() 标记的 MyReflectFunc
    UE_LOG(LogTemp, Log, TEXT("=== 遍历反射函数（UFunction） ==="));
    for (TFieldIterator<UFunction> FuncIt(MyClass); FuncIt; ++FuncIt)
    {
        UFunction* Func = *FuncIt;
        UE_LOG(LogTemp, Log, TEXT("反射函数：%s | 签名：%s"), *Func->GetName(), *Func->GetFunctionSignature());
        // 输出结果：反射函数：MyReflectFunc | 签名：void(int32)（MyNormalFunc 无 Field，不显示）
    }
}

```

## 核心总结（一句话说清宏标记的作用）

你写的 `UPROPERTY()`/`UFUNCTION()` 宏，本质是给 UHT 一个「标记信号」，让 UHT 自动生成反射代码，创建对应的 FField 子类对象（FProperty/UFunction），并将这些对象加入类的 Field 列表，最终让类对象「持有」这些 Field 成员，供 TFieldIterator 遍历——无宏标记的成员，没有对应的 Field 对象，自然无法被纳入和遍历。


> （注：文档部分内容可能由 AI 生成）