#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UICommon.h"
#include "Blueprint/UserWidget.h"
#include "UIPanelBase.generated.h"

class UUIPageBase;

UCLASS(Abstract)
class UUIPanelBase : public UObject
{
	GENERATED_BODY()

public:
	void Init(UUserWidget* InView, UUIPageBase* InOwnerPage);
	void Release();

	void Show();
	void Hide();

	UUserWidget* GetView() const { return View.Get(); }
	UUIPageBase* GetOwnerPage() const { return OwnerPage.Get(); }
	EUILifeState GetLifeState() const { return LifeState; }

protected:
	virtual void OnInit() {}
	virtual void OnShow() {}
	virtual void OnHide() {}
	virtual void OnRelease() {}

private:
	TWeakObjectPtr<UUserWidget> View;
	
	TWeakObjectPtr<UUIPageBase> OwnerPage;
	
	EUILifeState LifeState = EUILifeState::Initialized;
};