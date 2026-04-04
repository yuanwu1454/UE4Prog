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