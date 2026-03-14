// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyGame/Router/Room/RoomRouter.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYGAME_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ABasePlayerController();

	// ========== Router 访问器（对外唯一接口） ==========
	UFUNCTION(BlueprintCallable, Category = "Router")
	URoomRouter* GetRoomRouter() const { return RoomRouter; }
	

	// ========== 通用回调（所有 Router 共享） ==========
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestFailed, const FString&, Reason);
	UPROPERTY(BlueprintAssignable, Category = "Request")
	FOnRequestFailed OnRequestFailed;

	// 权限判断（所有 Router 共享）
	UFUNCTION(BlueprintCallable, Category = "Permission")
	bool IsAdmin() const { return bIsAdmin; }

protected:
	virtual void BeginPlay() override;

private:
	// ========== 各类 Router 实例（按业务归类） ==========
	UPROPERTY()
	URoomRouter* RoomRouter;
	

	// 玩家权限标识
	UPROPERTY()
	bool bIsAdmin = false;
};
