// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGame/Router/IGameRequestRouter.h"
#include "UObject/Object.h"
#include "RoomRouter.generated.h"


class ABasePlayerController;
class ARoomManagerActor;

UCLASS()
class MYGAME_API URoomRouter : public UObject, public IIGameRequestRouter
{
	GENERATED_BODY()
public:
	// ========== 房间类请求（所有房间接口都集中在这里） ==========
	UFUNCTION(BlueprintCallable, Category = "Room|Router")
	void CreateRoom(const FString& RoomName, int32 MaxPlayers, const FString& GameModeClass);
	
	// ========== 接口实现 ==========
	virtual void Init(ABasePlayerController* InPC) override;
	virtual bool ValidatePermission(const FString& RequestType) override;

	

private:
	// 所属的 PlayerController
	UPROPERTY()
	ABasePlayerController* OwnerPC;

	// 获取服务器 RoomManager
	ARoomManagerActor* GetRoomManager() const;
};
