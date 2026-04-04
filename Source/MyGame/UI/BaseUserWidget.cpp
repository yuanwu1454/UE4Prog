#include "BaseUserWidget.h"

#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UBaseUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UBaseUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

FString UBaseUserWidget::GetModuleName_Implementation() const
{
	return this->LuaModuleName;
}

void UBaseUserWidget::RestoreToGameMode()
{
	//
	// 方式1：通过创建UI的OwningPlayer获取（推荐，最安全）
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(PC))
		{
			MyPC->EnterGameOnlyInputMode();
		}
	}
}

APlayerController* UBaseUserWidget::GetSafePlayerController()
{
	return UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void UBaseUserWidget::CloseUI()
{
	RemoveFromParent();
	RestoreToGameMode();
}

UWorld* UBaseUserWidget::GetSelfWorld()
{
	return GetWorld();
}

bool UBaseUserWidget::HandleKeyEvent(FKey Key, EInputEvent InputEvent)
{
	auto LogHandleEvent = [&](bool Ret, const UBaseUserWidget* Widget)
	{
#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
		if (Ret)
		{
			UE_LOG(LogTemp, Log, TEXT("UPMBaseUserWidget::HandleKeyEvent %s Have Handle Key %s Event:%d"), *Widget->GetName(), *Key.GetFName().ToString(), InputEvent);
		}
#endif
	};

	if (!LuaModuleName.IsEmpty())
	{
		const bool bHandleRet = LuaHandleKeyEvent(Key, InputEvent);
		LogHandleEvent(bHandleRet, this);
		return bHandleRet;
	}
	return false;
}
