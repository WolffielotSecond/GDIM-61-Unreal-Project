// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class The_Awakening : ModuleRules
{
	public The_Awakening(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
            "GameplayAbilities",
            "GameplayTags",
			"GameplayTasks",
            "AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"Json",
			"JsonUtilities"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"The_Awakening",
			"The_Awakening/Variant_Platforming",
			"The_Awakening/Variant_Platforming/Animation",
			"The_Awakening/Variant_Combat",
			"The_Awakening/Variant_Combat/AI",
			"The_Awakening/Variant_Combat/Animation",
			"The_Awakening/Variant_Combat/Gameplay",
			"The_Awakening/Variant_Combat/Interfaces",
			"The_Awakening/Variant_Combat/UI",
			"The_Awakening/Variant_SideScrolling",
			"The_Awakening/Variant_SideScrolling/AI",
			"The_Awakening/Variant_SideScrolling/Gameplay",
			"The_Awakening/Variant_SideScrolling/Interfaces",
			"The_Awakening/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
