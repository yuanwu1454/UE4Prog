// IGameRequestRouter.h（接口类）
#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGameRequestRouter.generated.h"


class ABasePlayerController;


UINTERFACE(MinimalAPI)
class UIGameRequestRouter : public UInterface
{
	GENERATED_BODY()
};

class MYGAME_API IIGameRequestRouter
{
	GENERATED_BODY()

public:
	// 初始化 Router（绑定 PlayerController）
	virtual void Init(ABasePlayerController* InPC) = 0;

	// 校验请求权限（统一权限入口）
	virtual bool ValidatePermission(const FString& RequestType) = 0;
};