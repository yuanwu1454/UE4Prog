// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyPluginCommands.h"

#define LOCTEXT_NAMESPACE "FMyPluginModule"

void FMyPluginCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MyPlugin", "Execute MyPlugin action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
