#pragma once

namespace FieldToolNS
{
	FString GetBlueprintFunctionReturnType(UFunction* Func);
	FString GetFunctionFlagsString(EFunctionFlags Flags);
	FString GetPropertyTypeName(FProperty* Prop);
};
