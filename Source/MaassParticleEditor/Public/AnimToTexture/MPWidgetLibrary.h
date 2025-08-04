// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimToTexture/MPAnimToTextureDataAsset.h"
#include "MPAnimSequenceInfo.h"
#include "MPWidgetLibrary.generated.h"

class UMPAnimToTextureDataAsset;
class UMaterialFunction;
class UMaterialFunctionMaterialLayer;
class UMaterialInterface;
/**
 * A library of Blueprint-callable functions for editor utility widgets,
 * focusing on asset creation and material manipulation for the Maaass Particle system.
 */
UCLASS()
class MAAASSPARTICLEEDITOR_API UMPWidgetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

    /**
     * @brief Creates a new UMPAnimToTextureDataAsset or updates an existing one.
     * @details This editor-only utility function streamlines the creation and configuration of an AnimToTexture data asset.
     * It validates all inputs, constructs a standard asset name (e.g., DA_AssetName_BAT), and then either creates a new asset
     * at the specified path or loads and updates an existing one. The asset is populated with the provided skeletal mesh and
     * animations, and its properties are configured. Finally, the asset is saved to disk.
     *
     * @param PackagePath The content browser path where the data asset will be created (e.g., /Game/MyFolder). Must be a valid content path.
     * @param AssetName The base name for the data asset. A standard prefix ("DA_") and suffix ("_BAT") will be added.
     * @param SkeletalMesh The skeletal mesh to be assigned to the data asset.
     * @param SelectedAnims An array of animation sequences to be added to the data asset.
     * @param SampleRate The sample rate for the animations in the data asset.
     * @param NumDriverTriangles Number of triangles to use for the driver mesh.
     * @param Sigma The sigma value for the skinning decomposition.
     * @param bEnforcePowerOfTwo Whether to enforce power-of-two dimensions for the output textures.
     * @param Precision The precision (bit depth) for the output textures.
     * @param NumBoneInfluences The maximum number of bone influences to store per vertex.
     * @return A pointer to the newly created or updated UMPAnimToTextureDataAsset on success, or nullptr if the process fails.
     */
    UFUNCTION(BlueprintCallable, Category = "Maaass Particle Widget")
    static UMPAnimToTextureDataAsset* CreateMPAnimToTextureDA(
        const FString& PackagePath,
        const FString& AssetName,
        USkeletalMesh* SkeletalMesh,
        const TArray<FMPAnimSequenceInfo>& SelectedAnims,
        float SampleRate = 30,
        int32 NumDriverTriangles = 10,
        float Sigma = 1.0f,
        bool bEnforcePowerOfTwo = false,
        EAnimToTexturePrecision Precision = EAnimToTexturePrecision::EightBits,
        EAnimToTextureNumBoneInfluences NumBoneInfluences = EAnimToTextureNumBoneInfluences::Four
    );

    /*
     * @brief Copies a given FString to the system clipboard.
     * @details This is a simple helper function that uses the engine's platform-agnostic functionality
     * to copy a string to the user's operating system clipboard.
     *
     * @param TextToCopy The string that will be copied to the clipboard.
     */
    UFUNCTION(BlueprintCallable, Category = "Maaass Particle Widget")
    static void CopyTextToClipboard(const FString& TextToCopy);

    /*
     * @brief Checks if a string is a validly formatted Unreal Engine package path (e.g., "/Game/MyFolder").
     * @details The function verifies that the path starts with a valid root and contains no illegal characters,
     * making it safe to use for asset creation or lookups.
     *
     * @param PackagePath The path string to check.
     * @return True if the path is valid, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "Maaass Particle Widget")
    static bool IsValidPackagePath(const FString& PackagePath);

    /**
     * @brief Modifies a set of materials to support bone animation by inserting custom logic.
     * @details This function intercepts the material graph of each provided material. It handles two cases:
     * 1. Standard Materials: It re-routes the World Position Offset and Normal pins through a specified material function.
     * 2. Material Attribute Materials: It re-routes the final Material Attributes output through a specified material function.
     * A Material Attribute Layers node containing the BoneAnimationLayer is created and connected to the new function.
     * The function will not modify materials that already appear to have been processed.
     *
     * @param MaterialsToProcess The array of materials to modify.
     * @param BoneAnimationLayer The material layer function that contains the core bone animation logic.
     * @param StandardFunction The material function to insert for standard (non-attribute) materials.
     * @param AttributeFunction The material function to insert for materials using material attributes.
     * @param LayerInputName The name of the input pin on the functions that accepts the layer stack output.
     * @param WPOInputName The name of the input pin on the StandardFunction for the original World Position Offset.
     * @param WPOOutputName The name of the output pin on the StandardFunction for the new World Position Offset.
     * @param NormalInputName The name of the input pin on the StandardFunction for the original Normal.
     * @param NormalOutputName The name of the output pin on the StandardFunction for the new Normal.
     * @param Attribute_LayerInputName The name of the input pin on the AttributeFunction that accepts the layer stack output.
     * @param Attribute_MAInputName The name of the input pin on the AttributeFunction for the original Material Attributes.
     * @param Attribute_MAOutputName The name of the output pin on the AttributeFunction for the new Material Attributes.
     * @return The number of materials that were successfully modified.
     */
    UFUNCTION(BlueprintCallable, Category = "Maaass Particle Widget")
    static int32 SetupMaterialForBoneAnimation(
        const TArray<UMaterialInterface*>& MaterialsToProcess,
        UMaterialFunctionMaterialLayer* BoneAnimationLayer,
        UMaterialFunction* StandardFunction,
        UMaterialFunction* AttributeFunction,
        FName LayerInputName = "BoneAnimation",
        FName WPOInputName = "World Position Offset",
        FName WPOOutputName = "World Position Offset",
        FName NormalInputName = "Normal",
        FName NormalOutputName = "Normal",
        FName Attribute_LayerInputName = "BoneAnimation",
        FName Attribute_MAInputName = "Material Attributes",
        FName Attribute_MAOutputName = "Material Attributes"
    );

    /**
     * @brief Duplicates a Skeletal Mesh and all of its dependent materials to a new location.
     * @details This function performs a deep copy. It identifies all materials and material instances (including their parent hierarchy)
     * used by the SourceMesh, duplicates them to the TargetFolderPath, correctly re-parents the duplicated material instances,
     * and assigns the newly created materials to the newly created Skeletal Mesh.
     *
     * @param SourceMesh The Skeletal Mesh to duplicate.
     * @param TargetFolderPath The content browser path where the new assets will be created.
     * @param NewBaseAssetName The base name for the duplicated assets.
     * @param Suffix A suffix to append to all duplicated asset names (e.g., "_BAT").
     * @return A pointer to the newly created USkeletalMesh, or nullptr on failure.
     */
    UFUNCTION(BlueprintCallable, Category = "Maaass Particle Widget")
    static USkeletalMesh* DuplicateSkeletalMeshWithMaterials(
        USkeletalMesh* SourceMesh, 
        const FString& TargetFolderPath, 
        const FString& NewBaseAssetName, 
        const FString& Suffix = "_BAT"
    );

    /**
     * @brief Checks if a single material is a valid candidate for modification by SetupMaterialForBoneAnimation.
     * @details It returns false if the material appears to have already been processed (i.e., it already contains one of the
     * target function nodes or a material attribute layers node), preventing redundant modifications.
     *
     * @param MaterialToCheck The material to inspect.
     * @param LayerToCheck The layer that would be added (used for identification, not directly checked in this version).
     * @param StandardFunctionToCheck The standard function that would be added.
     * @param AttributeFunctionToCheck The attribute function that would be added.
     * @return True if the material can be safely processed, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "Maaass Particle Widget")
    static bool IsValidForSetupBoneAnimation(
        UMaterialInterface* MaterialToCheck,
        UMaterialFunctionMaterialLayer* LayerToCheck,
        UMaterialFunction* StandardFunctionToCheck,
        UMaterialFunction* AttributeFunctionToCheck
    );


    /**
     * @brief Checks if a batch of materials are all valid candidates for modification by SetupMaterialForBoneAnimation.
     * @details Iterates through the array and calls IsValidForSetupBoneAnimation on each material.
     * Returns false if any material in the batch is not valid for processing.
     *
     * @param MaterialToCheck The array of materials to inspect.
     * @param LayerToCheck The layer that would be added.
     * @param StandardFunctionToCheck The standard function that would be added.
     * @param AttributeFunctionToCheck The attribute function that would be added.
     * @return True if all materials in the batch can be safely processed, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category = "Maaass Particle Widget")
    static bool IsBatchValidForSetupBoneAnimation(
        TArray<UMaterialInterface*> MaterialToCheck,
        UMaterialFunctionMaterialLayer* LayerToCheck,
        UMaterialFunction* StandardFunctionToCheck,
        UMaterialFunction* AttributeFunctionToCheck
    );

    /**
     * @brief Finds and updates all active AMPSpawner actors in the current editor level.
     * @details This editor utility function iterates through all actors placed in the currently open
     * editor world (level). For each actor found that is of the AMPSpawner class, it calls the actor's
     * `UpdateNiagaraComponent` method. This is designed to be a global refresh mechanism, allowing an artist or designer
     * to apply recent changes (e.g., new data assets or material settings) to all relevant spawners
     * in the level with a single click, without needing to select each one individually.
     *
     * This function is particularly useful after generating new animation textures or modifying materials,
     * ensuring that all particle spawners in the scene are synchronized with the latest assets.
     */
    UFUNCTION(BlueprintCallable, Category = "Maaass Particle Widget")
    static void UpdateMPSpawnerNiagara();
};
