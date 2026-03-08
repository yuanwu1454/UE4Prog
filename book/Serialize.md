# 深入解析 UE 的 Serialize：对象持久化与数据传输

你的理解有一点点偏差——`Serialize` 并非「仅为 UAsset 准备」，也不是「业务程序员基本用不到」，而是**它的设计初衷是为 UE 的「对象持久化/数据传输」体系服务**（UAsset 只是其中一个场景）。业务程序员需要修改 `Serialize` 的核心场景，是当你需要**自定义 UE 内置序列化逻辑**时（比如处理非 `UPROPERTY` 数据、兼容版本迭代、优化序列化性能等）。

下面我会明确告诉你：**什么时候需要重写 ** **`Serialize`**、**什么时候不需要**，以及业务开发中最常见的实用场景。

### 一、先纠正核心认知：`Serialize` 的本质

`Serialize` 是 UE4 所有「对象数据读写」的底层入口，它的核心作用是：

- 定义「对象数据如何转化为字节流」（保存）；

- 定义「字节流如何还原为对象数据」（加载）。

UE4 会在以下场景自动调用 `Serialize`：

1. 保存/加载 UAsset 文件（核心场景）；

2. 保存/加载 SaveGame 存档（`USaveGame` 继承自 UObject）；

3. 网络同步（Actor/Component 的属性同步）；

4. 复制对象（`DuplicateObject`）；

5. 序列化到内存归档（如 `FMemoryArchive`）。

**关键区别**：

- 如果你用 UE 内置的「持久化体系」（UAsset/SaveGame/网络同步）→ `Serialize` 是核心，可重写扩展；

- 如果你想写「自定义纯二进制文件」（脱离 UE 体系）→ 不要用 `Serialize`（或仅用它处理业务数据，绝对不调 `Super::Serialize`）。

### 二、业务开发中，需要重写 `Serialize` 的核心场景

#### 场景1：需要序列化非 `UPROPERTY` 的数据（最常见）

UE 仅自动序列化标记 `UPROPERTY` 的属性，但如果你的类中有非 `UPROPERTY` 的成员（如纯 C++ 容器、自定义结构体），且需要这些数据随对象一起持久化/同步，就必须重写 `Serialize`。

**示例**：UAsset 中保存非 `UPROPERTY` 数据

```C++

UCLASS()
class UMyAsset : public UObject
{
    GENERATED_BODY()
public:
    // 非UPROPERTY数据（UE不会自动序列化）
    TMap<FString, int32> CustomMap; 

    virtual void Serialize(FArchive& Ar) override
    {
        Super::Serialize(Ar); // 保留UPROPERTY的自动序列化
        Ar << CustomMap;      // 手动序列化非UPROPERTY数据
    }
};
```

✅ 适用场景：制作自定义 UAsset（如配置表、关卡数据），需要存储 UE 反射系统不识别的纯 C++ 数据。

#### 场景2：处理序列化版本兼容（项目迭代必备）

项目迭代中，你可能会修改类的属性（如新增/删除字段），重写 `Serialize` 可以加入版本号判断，保证旧版本存档/UAsset 能正常加载。

**示例**：兼容不同版本的 UAsset/SaveGame

```C++

UCLASS(SaveGame)
class UMySaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    // 版本号（随项目迭代递增）
    static const int32 CurrentVersion = 2;

    UPROPERTY()
    FString PlayerName;
    // V2 新增属性
    UPROPERTY()
    int32 PlayerLevel; 

    virtual void Serialize(FArchive& Ar) override
    {
        Super::Serialize(Ar);
        
        int32 Version = CurrentVersion;
        if (Ar.IsLoading())
        {
            Ar << Version; // 加载时先读版本号
        }
        else
        {
            Ar << Version; // 保存时先写版本号
        }

        // 基础数据（所有版本都有）
        Ar << PlayerName;

        // V2 新增数据：仅版本≥2时加载
        if (Version >= 2)
        {
            Ar << PlayerLevel;
        }
        else if (Ar.IsLoading())
        {
            PlayerLevel = 1; // 旧版本默认值
        }
    }
};
```

✅ 适用场景：游戏存档（SaveGame）、自定义 UAsset 配置，需要兼容不同版本的旧数据。

#### 场景3：优化序列化性能（大型项目）

UE 自动序列化会遍历所有 `UPROPERTY`，如果你的类有大量属性但仅部分需要序列化（如临时数据无需保存），可重写 `Serialize` 手动控制，减少字节流大小，提升加载/网络同步速度。

**示例**：只序列化关键数据

```C++

UCLASS()
class UBigDataObject : public UObject
{
    GENERATED_BODY()
public:
    // 需要序列化的核心数据
    UPROPERTY()
    TArray<int32> CoreData;
    // 临时数据（无需序列化）
    TArray<float> TempCalculationData; 

    virtual void Serialize(FArchive& Ar) override
    {
        // 不调用Super::Serialize（避免序列化所有UPROPERTY）
        Ar << CoreData; // 仅序列化核心数据
        // TempCalculationData 不序列化，节省空间
    }
};
```

✅ 适用场景：大型场景数据、高频网络同步的对象（如多人游戏的角色属性）。

#### 场景4：自定义 UAsset 的序列化逻辑

如果你制作了自定义的编辑器资源（如自定义关卡数据、技能配置 UAsset），需要控制 UAsset 的保存格式，就必须重写 `Serialize`。

**示例**：自定义技能配置 UAsset

```C++

UCLASS(BlueprintType)
class USkillAsset : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    FString SkillName;
    // 非UPROPERTY的技能逻辑数据
    TArray<FName> SkillTags;

    virtual void Serialize(FArchive& Ar) override
    {
        Super::Serialize(Ar);
        Ar << SkillTags; // 让SkillTags随UAsset一起保存/加载
    }

    // 保存为UAsset的方法
    void SaveAsSkillAsset(const FString& AssetPath)
    {
        UPackage* Package = CreatePackage(*AssetPath);
        SetOuter(Package);
        Package->MarkPackageDirty();
        UPackage::SavePackage(Package, this, RF_Public|RF_Standalone, *FPackageName::LongPackageNameToFilename(AssetPath, TEXT(".uasset")));
    }
};
```

✅ 适用场景：编辑器内制作自定义资源，需要非 `UPROPERTY` 数据持久化。

### 三、什么时候不需要重写 `Serialize`？

1. **仅序列化 ** **`UPROPERTY`** ** 数据**：UE 会自动处理，无需重写；

2. **自定义纯二进制文件**：直接用 `FMemoryWriter`/`FFileHelper` 手动写字节流，不要用 `Serialize`（避免 UObject 元数据）；

3. **简单的 SaveGame**：仅标记 `UPROPERTY(SaveGame)`，UE 自动序列化，无需重写。

### 四、业务程序员的最佳实践

|开发需求|解决方案|
|---|---|
|游戏存档|继承 `USaveGame`，用 `UPROPERTY(SaveGame)` + 重写 `Serialize` 处理版本兼容|
|自定义编辑器资源（UAsset）|继承 `UObject`，重写 `Serialize` 处理非 `UPROPERTY` 数据|
|网络同步优化|重写 `Serialize`，仅序列化关键属性，减少带宽占用|
|自定义纯二进制配置文件|不用 `Serialize`，直接用 `FMemoryArchive` + `FFileHelper` 手动读写|
### 总结

1. **`Serialize`** ** 的核心价值**：为 UE 内置的「对象持久化/数据传输」体系（UAsset/SaveGame/网络同步）提供自定义扩展，而非为「纯二进制文件」设计；

2. **需要重写的场景**：处理非 `UPROPERTY` 数据、版本兼容、性能优化、自定义 UAsset；

3. **不需要重写的场景**：仅序列化 `UPROPERTY` 数据、自定义纯二进制文件；

4. **核心避坑点**：纯二进制文件不要调用 `Super::Serialize`（会写入 UObject 元数据导致解析错误），UAsset/SaveGame 则必须调用（否则 `UPROPERTY` 数据无法序列化）。

简单来说：**只要你的数据在 UE 的「对象体系」内流转（UAsset/SaveGame/网络同步），** **`Serialize`** ** 就是有用的；如果脱离 UE 体系（自定义二进制文件），就不要用它**。