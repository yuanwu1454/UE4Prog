#include "MyCoroutine.h"
// 

void FMyCoroutine::Run()
{
	check(Status == ERunStatus::NotRun)
	Status = ERunStatus::Running;
}

void FMyCoroutine::Tick(float DeltaTime)
{
	if (Status != ERunStatus::Running)
	{
		return;
	}
	while (RunIndex < ActionList.Num())
	{
		DynamicInsertIndex = RunIndex;
		const TPair<EActionFlag, FActionEntry>& Pair = ActionList[RunIndex];
		if (Pair.Key == EActionFlag::Flag_Action)
		{
			Pair.Value.Action.ExecuteIfBound();
			++RunIndex;
		}
		else if (Pair.Key == EActionFlag::Flag_WaitAction)
		{
			if (Pair.Value.WaitAction.IsBound() && !Pair.Value.WaitAction.Execute())
			{
				// 未能满足等待条件
				break;
			}
			++RunIndex;
		}
		else if (Pair.Key == EActionFlag::Flag_WaitFrame)
		{
			++AggregateIntValue;
			if (AggregateIntValue >= FMath::FloorToInt(Pair.Value.NumberValue + 0.4f))
			{
				++RunIndex;
				AggregateIntValue = 0;
			}
			else
			{
				break;
			}
		}
		else if (Pair.Key == EActionFlag::Flag_WaitSeconds)
		{
			AggregateFloatValue += DeltaTime;
			if (AggregateFloatValue >= Pair.Value.NumberValue)
			{
				++RunIndex;
				AggregateFloatValue = 0.0f;
			}
			else
			{
				break;
			}
		}
	}
	// 是否执行完
	if (RunIndex >= ActionList.Num())
	{
		// Finish
		Status = ERunStatus::Finish;
		RunIndex = 0;
		ActionList.Empty();
	}
}

void FMyCoroutine::Reset()
{
	Status = ERunStatus::NotRun;
	ActionList.Reset();
	RunIndex = 0;
}

void FMyCoroutine::Stop()
{
	Status = ERunStatus::Stopped;
}

void FMyCoroutine::ConfigNewAction(FCoroutineAction Action)
{
	ConfigToActionList(TPair<EActionFlag, FActionEntry>(Flag_Action, FActionEntry{0.0f, Action}));
}

void FMyCoroutine::ConfigWaitAction(FCoroutineWaitAction WaitAction)
{
	ConfigToActionList(TPair<EActionFlag, FActionEntry>(Flag_WaitAction, FActionEntry{0.0f, FCoroutineAction(), WaitAction}));
}

void FMyCoroutine::ConfigWaitForFrame(int FrameCount)
{
	ConfigToActionList(TPair<EActionFlag, FActionEntry>(Flag_WaitFrame, FActionEntry{static_cast<float>(FrameCount), FCoroutineAction()}));
}

void FMyCoroutine::ConfigWaitForSeconds(float Seconds)
{
	ConfigToActionList(TPair<EActionFlag, FActionEntry>(Flag_WaitSeconds, FActionEntry{Seconds, FCoroutineAction()}));
}

void FMyCoroutine::ConfigToActionList(TPair<EActionFlag, FActionEntry>&& Item)
{
	if (Status == ERunStatus::NotRun)
	{
		ActionList.Add(MoveTemp(Item));
	}
	else if (Status == ERunStatus::Running)
	{
		// 需要插入执行
		if (DynamicInsertIndex >= RunIndex)
		{
			if (DynamicInsertIndex < ActionList.Num() - 1)
			{
				ActionList.Insert(MoveTemp(Item), DynamicInsertIndex + 1);
			}
			else
			{
				ActionList.Add(MoveTemp(Item));
			}
			++DynamicInsertIndex;
		}
	}
}

// 全局协程对象
static FMyCoroutine GTestCoroutine;

// 测试用函数
void TestCoroutineAction1()
{
	UE_LOG(LogTemp, Log, TEXT("[协程测试] 执行动作 1"));
}

void TestCoroutineAction2()
{
	UE_LOG(LogTemp, Log, TEXT("[协程测试] 等待1秒后 → 执行动作 2"));
}

bool TestCoroutineWaitCondition()
{
	static float Timer = 0.f;
	Timer += 0.02f;
	if (Timer >= 2.f)
	{
		UE_LOG(LogTemp, Log, TEXT("[协程测试] 条件满足！继续执行"));
		Timer = 0.f;
		return true;
	}
	return false;
}

// 协程测试主逻辑
void RunTestCoroutine(UWorld* World)
{
	if (!World) return;

	UE_LOG(LogTemp, Log, TEXT("===== 启动协程测试 ====="));

	// 重置并配置
	GTestCoroutine.Reset();
	GTestCoroutine.ConfigNewAction(FCoroutineAction::CreateStatic(&TestCoroutineAction1));
	GTestCoroutine.ConfigWaitForSeconds(1.0f);
	GTestCoroutine.ConfigNewAction(FCoroutineAction::CreateStatic(&TestCoroutineAction2));
	GTestCoroutine.ConfigWaitAction(FCoroutineWaitAction::CreateStatic(&TestCoroutineWaitCondition));
	GTestCoroutine.ConfigWaitForFrame(5);
	GTestCoroutine.ConfigNewAction(FCoroutineAction::CreateLambda([]() {
		UE_LOG(LogTemp, Log, TEXT("[协程测试] ✅ 协程执行完毕！"));
	}));

	// 启动
	GTestCoroutine.Run();
}

// ====================== 关键：和你示例完全一样的控制台命令 ======================
static FAutoConsoleCommandWithWorldAndArgs TestCoroutineCmd(
	TEXT("TestCoroutine"),			// 控制台输入的命令
	TEXT("测试自定义协程"),			// 命令说明
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		// 执行协程测试
		RunTestCoroutine(World);
	})
);

// 自动注册每帧回调
static struct FCoroutineTicker
{
	FCoroutineTicker()
	{
		// 自动注册每帧更新
		FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float DeltaTime)
		{
			// 每帧自动驱动协程，完全自动，不需要任何Actor
			GTestCoroutine.Tick(DeltaTime);
			return true;
		}));
	}
} GCoroutineTicker;