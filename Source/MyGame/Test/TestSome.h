#pragma once
#include "Widgets/SWeakWidget.h"
#include "MySlateWidget.h"

// 引入UE4核心头文件（必加）
#include "Engine/Engine.h"
#include "Misc/StringFormatArg.h"
void TestString()
{
	FString Str1 = "Hello";
	FString Str2 = TEXT("Hello");
	const char* CStr = "Hello C String";
	FString Str3 = CStr;

	{
		int32 a = 1;
		uint32 d = 2;
		double f= 3.f;
		char x = 'c';
		int32* p = &a;
		
		// Str2 之所以要解引用，是因为支持的 const TCHAR* 类型
		UE_LOG(LogTemp, Log, TEXT("Str1 %s %d %u %f %c%p"), *Str2, a, d, f, x, p);

		const char* s = "asdf";
		// TEXT 格式化字符串是TCHAR类型
		// UE_LOG(LogTemp, Log, TEXT("Str1 %s"), s);
		UE_LOG(LogTemp, Log, TEXT("Str1 %s"), UTF8_TO_TCHAR(s));
		UE_LOG(LogTemp, Log, TEXT("Str1 %s"), *FString(UTF8_TO_TCHAR(s)));
		
		const TCHAR* ss = TEXT("zxvxcv");
		UE_LOG(LogTemp, Log, TEXT("Str1 %s"), ss);
	}

	{
		// 1. 定义带{0}/{1}/{2}占位符的基础字符串（UE4.27原生支持）
		FString BaseStr = TEXT("姓名：{0}，年龄：{1}，分数：{2:.2f}"); // {2:.2f} 可直接指定小数位数
		
		TArray<FStringFormatArg> Args;
		Args.Add(FStringFormatArg(TEXT("张三")));  // {0}
		Args.Add(FStringFormatArg(25));           // {1}
		Args.Add(FStringFormatArg(98.567f));       // {2}

		// 3. 调用Format（UE4.27标准写法）
		FString ResultStr = FString::Format(*BaseStr, Args);

		// 验证输出（结果："姓名：张三，年龄：25，分数：98.57"）
		UE_LOG(LogTemp, Log, TEXT("UE4.27格式化结果：%s"), *ResultStr);
	}

	{
		// ========== 场景1：命名参数（FStringFormatNamedArguments） ==========
		const TCHAR* NamedTemplate = TEXT("姓名：{Name}，年龄：{Age}，分数：{Score:.2f}");
		FStringFormatNamedArguments NamedArgs; // 确认用了别名
		NamedArgs.Add(TEXT("Name"), FStringFormatArg(TEXT("李四")));
		NamedArgs.Add(TEXT("Age"), FStringFormatArg(30));
		NamedArgs.Add(TEXT("Score"), FStringFormatArg(88.123f));

		// 核心：显式转换为const引用（强制匹配命名参数重载）
		const FStringFormatNamedArguments& NamedArgsRef = NamedArgs;
		FString NamedResult = FString::Format(NamedTemplate, NamedArgsRef);
		UE_LOG(LogTemp, Log, TEXT("命名参数结果：%s"), *NamedResult);

	}
}

