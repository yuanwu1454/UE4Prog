// RoomManagerActor.cpp
#include "RoomManagerActor.h"
#include "MyGame/BasePlayerController.h"
#include "Kismet/GameplayStatics.h"

ARoomManagerActor* ARoomManagerActor::Instance = nullptr;

ARoomManagerActor::ARoomManagerActor()
{
    // 关键：设置 Actor 为服务器常驻（网络标识必须开启）
    bReplicates = true;
    bAlwaysRelevant = true; // 对所有客户端可见
    PrimaryActorTick.bCanEverTick = false;
}

void ARoomManagerActor::BeginPlay()
{
    Super::BeginPlay();

    // 确保单例（服务器端）
    if (HasAuthority())
    {
        if (!Instance)
        {
            Instance = this;
        }
        else
        {
            Destroy(); // 销毁重复实例
        }
    }
}

// ========== 全局单例获取 ==========
ARoomManagerActor* ARoomManagerActor::GetRoomManager(UWorld* World)
{
    if (!World) return nullptr;

    // 服务器端直接返回单例
    if (Instance) return Instance;

    // 客户端查找场景中的 RoomManager
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, ARoomManagerActor::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        Instance = Cast<ARoomManagerActor>(FoundActors[0]);
        return Instance;
    }

    return nullptr;
}

// // ========== RPC：创建房间（服务器实现） ==========
// void ARoomManagerActor::Server_CreateRoom_Implementation(APlayerController* Requester, const FString& RoomName, int32 MaxPlayers, const FString& GameModeClass)
// {
//     FRoomInfo NewRoomInfo;
//     ERoomOperationResult Result = CreateRoom_Internal(RoomName, MaxPlayers, GameModeClass, NewRoomInfo);
//
//     // 向请求的客户端返回结果
//     Multicast_RoomOperationResult(Requester, Result, Result == ERoomOperationResult::Success ? NewRoomInfo.RoomID : "Invalid room");
// }
//
// bool ARoomManagerActor::Server_CreateRoom_Validate(APlayerController* Requester, const FString& RoomName, int32 MaxPlayers, const FString& GameModeClass)
// {
//     // 权限/参数校验（防止恶意请求）
//     return Requester && !RoomName.IsEmpty() && MaxPlayers >= 1 && MaxPlayers <= 16 && !GameModeClass.IsEmpty();
// }
//
// // ========== 内部业务逻辑：创建房间 ==========
// ERoomOperationResult ARoomManagerActor::CreateRoom_Internal(const FString& RoomName, int32 MaxPlayers, const FString& GameModeClass, FRoomInfo& OutRoomInfo)
// {
//     // 1. 校验房间名是否重复
//     for (const auto& Pair : ServerRoomMap)
//     {
//         if (Pair.Value.RoomName == RoomName)
//         {
//             return ERoomOperationResult::Failed_RoomExist;
//         }
//     }
//
//     // 2. 生成唯一 RoomID
//     FString RoomID = FString::Printf(TEXT("ROOM_%d_%d"), FMath::RandRange(1000, 9999), GetWorld()->GetTimeSeconds());
//
//     // 3. 存储房间信息
//     OutRoomInfo.RoomID = RoomID;
//     OutRoomInfo.RoomName = RoomName;
//     OutRoomInfo.MaxPlayers = MaxPlayers;
//     OutRoomInfo.CurrentPlayers = 1;
//     OutRoomInfo.bIsRoomOpen = true;
//     OutRoomInfo.GameModeClass = GameModeClass;
//
//     ServerRoomMap.Add(RoomID, OutRoomInfo);
//
//     UE_LOG(LogGame, Log, TEXT("Room created: %s (ID: %s)"), *RoomName, *RoomID);
//     return ERoomOperationResult::Success;
// }
//
// // ========== RPC：返回操作结果（客户端实现） ==========
// void ARoomManagerActor::Multicast_RoomOperationResult_Implementation(APlayerController* TargetPC, ERoomOperationResult Result, const FString& ExtraMsg)
// {
//     // 转发给 PlayerController 的回调（或直接触发 UI 委托）
//     if (ABasePlayerController* PC = Cast<ABasePlayerController>(TargetPC))
//     {
//         PC->OnRoomOperationResult.Broadcast(Result, ExtraMsg);
//     }
// }