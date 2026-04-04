#include "MyLuaBridgeObject.h"
// Sets default values
UMyLuaBridgeObject::UMyLuaBridgeObject()
{
}

void UMyLuaBridgeObject::CustomTick(float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_UMyLuaBridgeObject_CustomTick);
	AutoBindDelegates();
	TickLuaBridge(DeltaTime);
}

void UMyLuaBridgeObject::AutoBindDelegates()
{
	if (!bHaveAutoBind)
	{
		OnAutoRegist();
		bHaveAutoBind = true;
	}
}