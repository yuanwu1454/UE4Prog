#pragma once

namespace FieldToolNS
{
	FString GetBlueprintFunctionReturnType(UFunction* Func);
	FString GetFunctionFlagsString(EFunctionFlags Flags);
	FString GetPropertyTypeName(FProperty* Prop);
	// 传入任意UClass指针，判断其类型
	void CheckClassType(UClass* InClass);
	void PrintClassInfo(UClass* InClass);
};
