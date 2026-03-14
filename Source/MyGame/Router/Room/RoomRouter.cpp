// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomRouter.h"
// RoomRouter.cpp
#include "RoomRouter.h"
#include "MyGame/BasePlayerController.h"
#include "RoomManagerActor.h"

void URoomRouter::Init(ABasePlayerController* InPC)
{
	OwnerPC = InPC;
}

bool URoomRouter::ValidatePermission(const FString& RequestType)
{
	// 统一权限校验（比如管理员才能创建房间，普通玩家只能加入）
	if (!OwnerPC) return false;
	if (RequestType == "CreateRoom" && !OwnerPC->IsAdmin())
	{
		OwnerPC->OnRequestFailed.Broadcast("No permission to create room");
		return false;
	}
	return true;
}

void URoomRouter::CreateRoom(const FString& RoomName, int32 MaxPlayers, const FString& GameModeClass)
{
	// 1. 权限校验
	if (!ValidatePermission("CreateRoom")) return;

	// 2. 转发给服务器 RoomManager
	if (ARoomManagerActor* RoomMgr = GetRoomManager())
	{
		// RoomMgr->Server_CreateRoom(OwnerPC, RoomName, MaxPlayers, GameModeClass);
	}
}

ARoomManagerActor* URoomRouter::GetRoomManager() const
{
	return OwnerPC ? ARoomManagerActor::GetRoomManager(OwnerPC->GetWorld()) : nullptr;
}

// 其他房间接口（Join/Leave/RequestList）同理，都集中在这个 Router 中