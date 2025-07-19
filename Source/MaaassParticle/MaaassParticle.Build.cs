// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class MaaassParticle : ModuleRules
{
	public MaaassParticle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
			
			// Niagara VFX
			"Niagara",
			
			// Mass Framework
			"StructUtils",
            "MassEntity",
            "MassCommon",
            "MassMovement",
            "MassNavigation",
            "MassSimulation",
            "MassSpawner",
            "MassLOD",
            "MassAIBehavior",
            "StateTreeModule",

			// AnimToTexture (Runtime Part)
			"AnimToTexture",
			
			// Other Runtime Dependencies
			"RenderCore",
            "RHI",
            "VectorVM",
            "ApplicationCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {

		});

        PublicIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Public/MassEntity/Fragments"),
            Path.Combine(ModuleDirectory, "Public/MassEntity/Processors"),
            Path.Combine(ModuleDirectory, "Public/MassEntity/Traits"),
            Path.Combine(ModuleDirectory, "Public/NiagaraDataInterface"),
        });

        PrivateIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Private/MassEntity/Fragments"),
            Path.Combine(ModuleDirectory, "Private/MassEntity/Processors"),
            Path.Combine(ModuleDirectory, "Private/MassEntity/Traits"),
            Path.Combine(ModuleDirectory, "Private/NiagaraDataInterface"),
        });
    }
}
