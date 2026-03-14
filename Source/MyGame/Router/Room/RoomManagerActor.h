// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomManagerActor.generated.h"

UCLASS()
class MYGAME_API ARoomManagerActor : public AActor
{
	GENERATED_BODY()

public:
    ARoomManagerActor();

    // ========== 全局单例获取（关键） ==========
    UFUNCTION(BlueprintCallable, Category = "Room|Manager")
    static ARoomManagerActor* GetRoomManager(UWorld* World);

    // ========== 客户端可调用的RPC接口 ==========
    // 客户端→服务器：创建房间（RPC 直接定义在 Actor 上）
    // UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Room|RPC")
    // void Server_CreateRoom(APlayerController* Requester, const FString& RoomName, int32 MaxPlayers, const FString& GameModeClass);
    // void Server_CreateRoom_Implementation(APlayerController* Requester, const FString& RoomName, int32 MaxPlayers, const FString& GameModeClass);
    // bool Server_CreateRoom_Validate(APlayerController* Requester, const FString& RoomName, int32 MaxPlayers, const FString& GameModeClass);

    // 客户端→服务器：加入房间
    // UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Room|RPC")
    // void Server_JoinRoom(APlayerController* Requester, const FString& RoomID);
    // void Server_JoinRoom_Implementation(APlayerController* Requester, const FString& RoomID);
    // bool Server_JoinRoom_Validate(APlayerController* Requester, const FString& RoomID);

    // 服务器→指定客户端：返回操作结果
    // UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Room|RPC")
    // void Multicast_RoomOperationResult(APlayerController* TargetPC, ERoomOperationResult Result, const FString& ExtraMsg);
    // void Multicast_RoomOperationResult_Implementation(APlayerController* TargetPC, ERoomOperationResult Result, const FString& ExtraMsg);

protected:
    virtual void BeginPlay() override;

private:
    // 服务器端存储所有房间数据
    // TMap<FString, FRoomInfo> ServerRoomMap;

    // 单例实例
    static ARoomManagerActor* Instance;

    // // 内部业务逻辑：创建房间（无RPC，纯逻辑）
    // ERoomOperationResult CreateRoom_Internal(const FString& RoomName, int32 MaxPlayers, const FString& GameModeClass, FRoomInfo& OutRoomInfo);

    // // 内部业务逻辑：校验房间是否存在/满员
    // ERoomOperationResult ValidateRoom(const FString& RoomID, const FRoomInfo** OutRoomInfo = nullptr);
};
