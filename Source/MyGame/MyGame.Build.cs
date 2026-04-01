// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;
using System.IO;

public class MyGame : ModuleRules
{
	public MyGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"UMG",
			"SlateCore",
			"Slate",
			"OnlineSubsystem",
			"OnlineSubsystemNull",
			"UnLua",
		});
		
		// ✨ 优雅的头文件路径配置
		PublicIncludePaths.AddRange(new[]
		{
			ModuleDirectory,
			Path.Combine(ModuleDirectory, "UI"),
			Path.Combine(ModuleDirectory, "Global"),
			Path.Combine(ModuleDirectory, "Component"),
			Path.Combine(ModuleDirectory, "Assets"),
			Path.Combine(ModuleDirectory, "CSV"),
			Path.Combine(ModuleDirectory, "Log"),
			Path.Combine(ModuleDirectory, "Slate"),
		});
		
		// 关键：如果是编辑器模块，添加Editor相关依赖
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { 
				"UnrealEd",    // 编辑器核心
				"EditorStyle"  // 编辑器样式（可选，视需求）
			});
		}
		
		
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
