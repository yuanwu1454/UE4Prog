#pragma once
#include "CoreMinimal.h"
#include "MyViewController.h"
#include "UObject/Object.h"
#include "UICommon.h"
#include "Blueprint/UserWidget.h"
#include "MyViewControllerPanel.generated.h"

UCLASS()
class UMyViewControllerPanel : public UMyViewController
{
	GENERATED_BODY()

public:
	void Init(UUserWidget* InView, UMyViewController* InOwnerPage);
	virtual void Release() override;

	void Show();
	void Hide();
	
	UMyViewController* GetOwnerPage() const { return OwnerPage.Get(); }
protected:
	virtual void OnRelease() {}

private:
	TWeakObjectPtr<UMyViewController> OwnerPage;
};