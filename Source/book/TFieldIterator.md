你想知道 TFieldIterator 除了遍历 UFunction（函数）和 FProperty（属性）之外，还能遍历哪些类型，
其实 TFieldIterator 是 Unreal 反射系统中用于遍历 UClass/UBlueprintGeneratedClass 中所有
「字段（Field）」 的通用迭代器，核心支持所有继承自 FField 的类型，覆盖 UE 类中几乎所有可反射的成员。

1. TFieldIterator 的核心是遍历 FField 子类，最常用的是 UFunction（函数）和 FProperty（所有属性）；
2. 针对 FProperty 可进一步筛选遍历：FArrayProperty（数组）、FObjectProperty（Object 引用）、FEnumProperty（枚举）等；
3. 开发中可根据需求选择「基类遍历（全量）」或「子类遍历（精准筛选）」，结合 EFieldIteratorFlags 可控制是否遍历父类字段。 

### 三、关键使用技巧
1. 基类 vs 子类遍历：
   用 TFieldIterator<FProperty> 会遍历所有类型的属性（数组、结构体、Object 引用等），因为 FProperty 是所有属性的基类；
   用 TFieldIterator<FArrayProperty> 只会筛选出数组类型的属性，相当于「精准过滤」。
   遍历范围控制：
2. TFieldIterator 支持第二个参数控制遍历范围（UE4.27）：
   ```cpp
   // 第二个参数传 EFieldIteratorFlags::ExcludeSuper 表示「不遍历父类的字段」
   for (TFieldIterator<UFunction> FuncIt(WidgetClass, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
   {
   // 只遍历当前类自己定义的函数，不包含父类（如 UUserWidget）的函数
   }
   ```
3. 空值判断：
   遍历过程中一定要加 if (!XXX) continue;，因为部分反射字段可能为空（比如未初始化的嵌套结构体）。


* UField vs FProperty 的区别：
   UField 是「UObject 系」的 FField 子类（继承了 UObject），比如 UFunction、UDelegateProperty，支持垃圾回收、蓝图反射；
   FProperty 是「非 UObject 系」的 FField 子类，比如 FIntProperty、FArrayProperty，轻量级，不占 UObject 内存池。
   但不管是 UField 还是 FProperty，都继承 FField，所以都能被 TFieldIterator 遍历。
* 为什么有些类型不能被遍历？
  只有注册到 UE 反射系统、且继承 FField 的类型才能被遍历。比如：
  C++ 普通成员变量（未加 UPROPERTY）→ 未注册为 FProperty → 无法遍历；
  C++ 普通函数（未加 UFUNCTION）→ 未注册为 UFunction → 无法遍历。