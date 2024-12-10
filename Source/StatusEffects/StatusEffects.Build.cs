// Copyright No Bright Shadows. All Rights Reserved.

using UnrealBuildTool;

// ReSharper disable UseCollectionExpression
// ReSharper disable InvertIf

// ReSharper disable once InconsistentNaming
public class StatusEffects : ModuleRules
{
	// ReSharper disable once InconsistentNaming
	public StatusEffects(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(new string[]
		{
			
		});
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "GameplayAbilities", "GameplayTags", "GameplayTasks", "UE5Coro", "NetCore", "CommonUI", "UMG"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject", "Engine", "Slate", "SlateCore"
		});
	}
}
