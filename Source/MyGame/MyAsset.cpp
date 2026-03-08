// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAsset.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"
#include "Editor/UnrealEd/Public/Editor.h" // 需添加UnrealEd模块依赖
// 辅助：初始化测试数据
void UMyAsset::InitTestData()
{
    // 模拟添加测试数据到CustomMap
    MapKey = TEXT("HP");
    MapValue = 100;
    AddToCustomMap();

    MapKey = TEXT("MP");
    MapValue = 80;
    AddToCustomMap();

    MapKey = TEXT("ATK");
    MapValue = 50;
    AddToCustomMap();
}

UMyAsset* UMyAsset::CreateAndSaveAsUAsset(const FString& AssetPath)
{
    // 1. 校验路径
    if (!AssetPath.StartsWith(TEXT("/Game/")))
    {
        UE_LOG(LogTemp, Error, TEXT("UAsset路径必须以/Game/开头！示例：/Game/MyAssets/TestAsset"));
        if (GEditor)
        {
            FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("UAsset路径必须以/Game/开头！")));
        }
        return nullptr;
    }

    // 2. 创建Package
    UPackage* Package = CreatePackage(*AssetPath);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("创建Package失败：%s"), *AssetPath);
        return nullptr;
    }

    // ========== 关键修改：提取Package名称作为对象名称 ==========
    FString ObjectName = FPackageName::GetShortName(AssetPath); // 从路径提取名称（如TestCustomMapAsset_UE427）
    FName ObjName = FName(*ObjectName);

    // 3. 创建对象时指定固定名称（避免自动生成MyAsset_0）
    UMyAsset* NewAsset = NewObject<UMyAsset>(
        Package,                  // Outer
        UMyAsset::StaticClass(),  // 类类型
        ObjName,                  // 固定对象名称（和Package同名）
        RF_Public | RF_Standalone // 标记位
    );

    if (!NewAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("创建UMyAsset实例失败"));
        return nullptr;
    }

    // 4. 保存Package（不变）
    Package->MarkPackageDirty();
    const FString PackageFileName = FPackageName::LongPackageNameToFilename(
        AssetPath,
        FPackageName::GetAssetPackageExtension()
    );

    bool bSaved = UPackage::SavePackage(
        Package,
        NewAsset,
        RF_Public | RF_Standalone,
        *PackageFileName,
        GError,
        nullptr,
        true
    );

    if (bSaved)
    {
        UE_LOG(LogTemp, Log, TEXT("UAsset保存成功！路径：%s，对象名称：%s"), *AssetPath, *ObjectName);
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
        AssetRegistryModule.Get().AssetCreated(NewAsset);
        return NewAsset;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UAsset保存失败：%s"), *AssetPath);
        return nullptr;
    }
}
// 辅助函数：加载UAsset中的UMyAsset对象（兼容任意名称）
static UMyAsset* LoadMyAssetFromPackage(const FString& AssetPath)
{
    // 方式1：先尝试加载固定名称的对象（Package路径 + 对象名称）
    FString ObjectPath = AssetPath + TEXT(".") + FPackageName::GetShortName(AssetPath);
    UMyAsset* Asset = LoadObject<UMyAsset>(nullptr, *ObjectPath);
    if (Asset)
    {
        return Asset;
    }

    // 方式2：如果固定名称找不到，遍历Package内的所有对象
    UPackage* Package = LoadObject<UPackage>(nullptr, *AssetPath);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("加载Package失败：%s"), *AssetPath);
        return nullptr;
    }

    // 遍历Package内的所有对象，找到UMyAsset类型
    for (TObjectIterator<UMyAsset> It; It; ++It)
    {
        UMyAsset* CurrentAsset = *It;
        if (CurrentAsset && CurrentAsset->GetOutermost() == Package)
        {
            UE_LOG(LogTemp, Log, TEXT("找到Package内的UMyAsset对象：%s"), *CurrentAsset->GetName());
            return CurrentAsset;
        }
    }

    UE_LOG(LogTemp, Error, TEXT("Package内未找到UMyAsset对象：%s"), *AssetPath);
    return nullptr;
}

// 重写测试函数的步骤5
void UMyAsset::TestCustomMapSerialization()
{
    // ========== 步骤1：定义UAsset路径 ==========
    const FString AssetPath = TEXT("/Game/MyAssets/TestCustomMapAsset_UE427");

    // ========== 步骤2：创建并保存UAsset ==========
    UMyAsset* TestAsset = CreateAndSaveAsUAsset(AssetPath);
    if (!TestAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("创建并保存UAsset失败"));
        return;
    }

    // ========== 步骤3：初始化测试数据 ==========
    TestAsset->InitTestData();

    // 验证C++读取
    int32 HP = TestAsset->GetFromCustomMap(TEXT("HP"));
    int32 MP = TestAsset->GetFromCustomMap(TEXT("MP"));
    int32 ATK = TestAsset->GetFromCustomMap(TEXT("ATK"));
    UE_LOG(LogTemp, Log, TEXT("【测试步骤3】C++读取CustomMap：HP=%d, MP=%d, ATK=%d"), HP, MP, ATK);

    // ========== 步骤4：重新保存 ==========
    UPackage* Package = TestAsset->GetOutermost();
    if (Package)
    {
        Package->MarkPackageDirty();
        const FString PackageFileName = FPackageName::LongPackageNameToFilename(AssetPath, FPackageName::GetAssetPackageExtension());
        UPackage::SavePackage(Package, TestAsset, RF_Public | RF_Standalone, *PackageFileName, GError, nullptr, true);
        UE_LOG(LogTemp, Log, TEXT("【测试步骤4】重新保存UAsset，包含测试数据"));
    }

    // ========== 步骤5：修复后的加载逻辑 ==========
    UMyAsset* LoadedAsset = LoadMyAssetFromPackage(AssetPath); // 调用辅助加载函数

    if (LoadedAsset)
    {
        int32 LoadedHP = LoadedAsset->GetFromCustomMap(TEXT("HP"));
        int32 LoadedMP = LoadedAsset->GetFromCustomMap(TEXT("MP"));
        int32 LoadedATK = LoadedAsset->GetFromCustomMap(TEXT("ATK"));
        TArray<FString> AllKeys = LoadedAsset->GetAllMapKeys();

        UE_LOG(LogTemp, Log, TEXT("【测试步骤5】从UAsset加载后：HP=%d, MP=%d, ATK=%d"), LoadedHP, LoadedMP, LoadedATK);
        UE_LOG(LogTemp, Log, TEXT("【测试步骤5】CustomMap所有Key："));
        for (const FString& Key : AllKeys)
        {
            UE_LOG(LogTemp, Log, TEXT("  - %s"), *Key);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("【测试步骤5】加载UAsset失败：%s"), *AssetPath);
    }

    // 屏幕提示
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, 
            TEXT("UMyAsset测试完成！查看日志验证结果"));
    }
}