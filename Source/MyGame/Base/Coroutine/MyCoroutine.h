#pragma once

#include "CoreTypes.h"
#include "MyCoroutine.generated.h"

DECLARE_DELEGATE(FCoroutineAction);
DECLARE_DELEGATE_RetVal(bool, FCoroutineWaitAction);

// 虚拟协程
USTRUCT()
struct FMyCoroutine
{
GENERATED_BODY()
	enum EActionFlag
	{
		Flag_Action,
		Flag_WaitAction,
		Flag_WaitFrame,
		Flag_WaitSeconds
	};

	struct FActionEntry
	{
		float NumberValue = 0.0f;
		FCoroutineAction Action = FCoroutineAction();
		FCoroutineWaitAction WaitAction = FCoroutineWaitAction();
	};

	enum ERunStatus
	{
		NotRun,
		Running,
		Finish,
		Stopped
	};
	void Run();
	void Tick(float DeltaTime);
	void Reset();
	void Stop();
	/*************** Config配置接口*********************/
	// Config时，如果未运行则顺序向下执行；如果已经执行则播放执行。
	void ConfigNewAction(FCoroutineAction Action);
	void ConfigWaitAction(FCoroutineWaitAction WaitAction);
	void ConfigWaitForFrame(int FrameCount);
	void ConfigWaitForSeconds(float Seconds);

	void ConfigToActionList(TPair<EActionFlag, FActionEntry>&& Item);

	ERunStatus Status = ERunStatus::NotRun;
	uint8 RunIndex = 0;
	uint8 DynamicInsertIndex = 0;
	float AggregateFloatValue = 0.0f;
	float AggregateIntValue = 0;
	TArray<TPair<EActionFlag, FActionEntry>> ActionList;
};

// 在 UE4 里：
// 没有 XXX_API 的类 = 只能在引擎内部使用 = 外部模块绝对不能继承！
// 不管你重写不重写，都不能用！