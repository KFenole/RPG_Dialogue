// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class RPG_Dialogue : ModuleRules
{
	public RPG_Dialogue(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "RuntimeAudioImporter", "XmlParser" });

		//PublicIncludePaths.AddRange(new string[] { "Plugins/RuntimeAudioImporter/Source/RuntimeAudioImporter/Public" });
		//PublicIncludePaths.Add(Path.Combine("Plugins/RuntimeAudioImporter", "/Source/RuntimeAudioImporter/Public"));

	}
}
