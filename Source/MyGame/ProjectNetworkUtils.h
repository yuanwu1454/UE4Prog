// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNetworkUtils.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API UProjectNetworkUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	 /**
     * 判断当前是否为服务器（专用服务器/Listen Server）
     * @param World 世界上下文（必填，用于获取网络模式）
     * @return true = 专用服务器/Listen Server；false = 纯客户端/单机
     */
    UFUNCTION(BlueprintPure, Category = "Network|Utils", meta = (WorldContext = "World"))
    static FORCEINLINE bool IsServer(const UObject* WorldContextObject)
    {
        if (!IsValid(WorldContextObject)) return false;
        UWorld* World = WorldContextObject->GetWorld();
        if (!IsValid(World)) return false;

        ENetMode NetMode = World->GetNetMode();
        return NetMode == NM_DedicatedServer || NetMode == NM_ListenServer;
    }

    // 扩展：常用的其他网络模式判断（可选，按需添加）
    /** 判断是否为专用服务器（纯服务器，无客户端界面） */
    UFUNCTION(BlueprintPure, Category = "Network|Utils", meta = (WorldContext = "World"))
    static FORCEINLINE bool IsDedicatedServer(const UObject* WorldContextObject)
    {
        if (!IsValid(WorldContextObject)) return false;
        UWorld* World = WorldContextObject->GetWorld();
        return IsValid(World) && World->GetNetMode() == NM_DedicatedServer;
    }

    /** 判断是否为Listen Server（主机/带客户端界面的服务器） */
    UFUNCTION(BlueprintPure, Category = "Network|Utils", meta = (WorldContext = "World"))
    static FORCEINLINE bool IsListenServer(const UObject* WorldContextObject)
    {
        if (!IsValid(WorldContextObject)) return false;
        UWorld* World = WorldContextObject->GetWorld();
        return IsValid(World) && World->GetNetMode() == NM_ListenServer;
    }

    /** 判断是否为纯客户端（非服务器/非主机） */
    UFUNCTION(BlueprintPure, Category = "Network|Utils", meta = (WorldContext = "World"))
    static FORCEINLINE bool IsPureClient(const UObject* WorldContextObject)
    {
        if (!IsValid(WorldContextObject)) return false;
        UWorld* World = WorldContextObject->GetWorld();
        return IsValid(World) && World->GetNetMode() == NM_Client;
    }

    /** 判断是否为单机模式（无网络） */
    UFUNCTION(BlueprintPure, Category = "Network|Utils", meta = (WorldContext = "World"))
    static FORCEINLINE bool IsStandalone(const UObject* WorldContextObject)
    {
        if (!IsValid(WorldContextObject)) return false;
        UWorld* World = WorldContextObject->GetWorld();
        return IsValid(World) && World->GetNetMode() == NM_Standalone;
    }
};
