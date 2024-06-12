// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PirateWar : ModuleRules
{
	public PirateWar(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
			"EnhancedInput", "OnlineSubsystem", "OnlineSubsystemSteam", "Niagara", "MultiplayerSessions" });
	}
}
