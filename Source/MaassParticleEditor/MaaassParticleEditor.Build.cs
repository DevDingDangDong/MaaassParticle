// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class MaaassParticleEditor : ModuleRules
{
	public MaaassParticleEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[]{
            Path.Combine(ModuleDirectory, "Public"),
            Path.Combine(ModuleDirectory, "Public", "AnimToTexture"),
        });

        PrivateIncludePaths.AddRange(new string[]{
            Path.Combine(ModuleDirectory, "Private"),
            Path.Combine(ModuleDirectory, "Private", "AnimToTexture"),
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "ApplicationCore",
			"UnrealEd",
			"ToolMenus",
            "MaaassParticle",
            "AnimToTexture"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            
			"AnimToTextureEditor",
			"MaterialEditor",
            "MeshUtilities",
            "MeshDescription",
            "StaticMeshDescription",
			
			"Slate",
            "SlateCore",
            "PropertyEditor",
            "EditorStyle",

            //Widget Tab 
            "Blutility",             
            "UMG",                   
            "UMGEditor",             
            "WorkspaceMenuStructure",
            "StaticMeshEditor",
            "LevelEditor",
            "EditorSubsystem"
        });

    }
}
