// Fill out your copyright notice in the Description page of Project Settings.


#include "MyObject.h"

#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Engine/Engine.h"

UMyObject::UMyObject()
{
    // 初始化自定义数据

    // UObject默认会自动注册到GC系统，无需手动管理内存
}

void UMyObject::PrintObjectInfo()
{
    FString Info = FString::Printf(TEXT("MyString: %s, MyInt: %d, MyFloat: %.2f"), 
        *MyString, MyInt, MyFloat);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Info);
    }
    UE_LOG(LogTemp, Log, TEXT("UMyObject Info: %s"), *Info);

    for (auto Ele : CustomData)
    {
        UE_LOG(LogTemp, Log, TEXT("%s"), *Ele.ToString());
    }
}

void UMyObject::Serialize(FArchive& Ar)
{
    // 第一步：调用父类的Serialize，确保UPROPERTY的自动序列化
    Super::Serialize(Ar);

    // // 第二步：自定义序列化逻辑（处理非UPROPERTY的成员）
    // if (Ar.IsSaving())
    // {
    //     // 保存时：将CustomData写入归档
    //     Ar << CustomData;
    //     UE_LOG(LogTemp, Log, TEXT("UMyObject 保存自定义数据，数量：%d"), CustomData.Num());
    // }
    // else if (Ar.IsLoading())
    // {
    //     // 加载时：从归档读取CustomData
    //     Ar << CustomData;
    //     UE_LOG(LogTemp, Log, TEXT("UMyObject 加载自定义数据，数量：%d"), CustomData.Num());
    // }
}

void UMyObject::SaveToConfig()
{
    // UObject的Config能力：将标记为Config的UPROPERTY写入配置文件
    // Config=Game 对应 Saved/Config/Windows/Game.ini
    GConfig->SetString(
        TEXT("/Script/MyProject.MyObject"), // 配置节（格式：/Script/模块名.类名）
        TEXT("MyString"),                  // 配置项名称
        *MyString,                          // 要保存的值
        GGameIni                           // 配置文件类型（Game.ini）
    );
    GConfig->Flush(false, GGameIni); // 刷盘保存
    UE_LOG(LogTemp, Log, TEXT("UMyObject 保存到Config：%s"), *MyString);
}

void UMyObject::LoadFromConfig()
{
    // 从Config文件加载标记为Config的UPROPERTY
    FString LoadedString;
    if (GConfig->GetString(
        TEXT("/Script/MyProject.MyObject"),
        TEXT("MyString"),
        LoadedString,
        GGameIni
    ))
    {
        MyString = LoadedString;
        UE_LOG(LogTemp, Log, TEXT("UMyObject 从Config加载：%s"), *MyString);
    }
}

void UMyObject::BeginDestroy()
{
    // 对象被GC销毁前的自定义清理逻辑
    UE_LOG(LogTemp, Log, TEXT("UMyObject 开始销毁，对象名称 %s"), *GetName());
    CustomData.Empty(); // 清理自定义数据
    Super::BeginDestroy(); // 必须调用父类，否则会导致GC异常
}
// 放在MyObject.cpp中，作为UMyObject的成员函数（或全局工具函数）
/** 保存对象到二进制文件（序列化） */
void UMyObject::SaveToFile(const FString& FilePath)
{
    // 创建写入归档
    FArchive* SaveAr = IFileManager::Get().CreateFileWriter(*FilePath);
    if (SaveAr )
    {
        // 执行序列化（包括UPROPERTY和自定义非UPROPERTY）
        this->Serialize(*SaveAr);
        SaveAr->Flush();
        SaveAr->Close();
        delete SaveAr; // 手动释放归档
        UE_LOG(LogTemp, Log, TEXT("对象已保存到文件：%s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("创建写入文件失败：%s"), *FilePath);
    }
}

/** 从二进制文件加载（反序列化） */
void UMyObject::LoadFromFile(const FString& FilePath)
{
    // 检查文件是否存在
    if (!FPaths::FileExists(FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("文件不存在：%s"), *FilePath);
        return;
    }

    // 创建读取归档
    FArchive* LoadAr = IFileManager::Get().CreateFileReader(*FilePath);
    if (LoadAr)
    {
        // 执行反序列化（会自动填充UPROPERTY和自定义非UPROPERTY）
        this->Serialize(*LoadAr);
        LoadAr->Close();
        delete LoadAr; // 手动释放归档
        UE_LOG(LogTemp, Log, TEXT("对象已从文件加载：%s"), *FilePath);
        // 加载后验证数据
        this->PrintObjectInfo();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("创建读取文件失败：%s"), *FilePath);
    }
}

void UMyObject::Test()
{
    // 1. 创建UMyObject实例（必须通过NewObject，不能用new）
    UMyObject* MyObj = NewObject<UMyObject>();

    // 2. 修改属性
    MyObj->MyString = TEXT("测试字符串");
    MyObj->MyInt = 210;
    MyObj->CustomData.Add("A1");
    MyObj->CustomData.Add("A2");

    // 3. 调用反射标记的函数
    MyObj->PrintObjectInfo();

    // 4. 保存到配置文件
    MyObj->SaveToConfig();

    // 1. 保存到文件
    FString SavePath = FPaths::ProjectSavedDir() / TEXT("MyObj_Save.bin");
    // MyObj->SaveToFile(SavePath);

    // // 2. 新建对象，从文件反序列化加载
    UMyObject* LoadedObj = NewObject<UMyObject>();
    LoadedObj->LoadFromFile(SavePath);
    //
    // // 6. 释放对象（GC自动处理，无需手动delete）
    // MyObj = nullptr; // 移除强引用，GC会在合适时机销毁
    // LoadedObj = nullptr;
}
