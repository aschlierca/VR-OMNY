// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VR_OMNY : ModuleRules
{
	public VR_OMNY(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"SlateCore"
		});
		bEnableExceptions = true;

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"VR_OMNY",
			"VR_OMNY/Variant_Horror",
			"VR_OMNY/Variant_Horror/UI",
			"VR_OMNY/Variant_Shooter",
			"VR_OMNY/Variant_Shooter/AI",
			"VR_OMNY/Variant_Shooter/UI",
			"VR_OMNY/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
