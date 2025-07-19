// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MPAnimToTextureBPLibrary.generated.h"

struct FAnimToTextureAnimSequenceInfo;
class UMPAnimToTextureDataAsset;

/**
 * Blueprint Function Library for converting skeletal mesh animations to textures with LODs
 */
UCLASS()
class MAAASSPARTICLEEDITOR_API UMPAnimToTextureBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Converts a skeletal mesh to a static mesh with all LOD levels preserved.
     * Creates a new static mesh asset with the specified materials applied.
     * If the target static mesh already exists, it updates the existing mesh instead.
     * 
     * @param DataAsset The data asset containing the skeletal mesh to convert
     * @param Materials Array of materials to apply to the converted static mesh
     * @param PackagePath The package path where the static mesh will be saved
     * @param AssetName The name for the new static mesh asset (will be prefixed with "SM_")
     * @return The created or updated static mesh, nullptr if conversion failed
     */
    UFUNCTION(BlueprintCallable, meta = (Category = "AnimToNiagara"))
    static UStaticMesh* ConvertSkeletalMeshToStaticMeshWithLODs(UMPAnimToTextureDataAsset* DataAsset, TArray<UMaterialInterface*> Materials, FString PackagePath, FString AssetName);

    /**
     * Configures lightmap settings for all LOD levels of a static mesh.
     * Sets up UV channels and build settings required for proper lightmap generation.
     * 
     * @param StaticMesh The static mesh to configure lightmap settings for
     * @param LightmapIndex The UV channel index to use for lightmap coordinates
     * @param bGenerateLightmapUVs Whether to automatically generate lightmap UVs during build
     * @return The modified static mesh with updated lightmap settings
     */
    UFUNCTION(BlueprintCallable, meta = (Category = "AnimToNiagara"))
    static UStaticMesh* SetLightMapWithLODs(UStaticMesh* StaticMesh, const int32 LightmapIndex, bool bGenerateLightmapUVs);
    
    /**
     * Creates bone weight textures for all LOD levels of the static mesh in the data asset.
     * Generates position, rotation, and weight textures required for GPU-based animation.
     * Each LOD level gets its own weight texture while sharing position and rotation textures.
     * 
     * @param DataAsset The data asset containing mesh and animation data
     * @param PackagePath The package path where textures will be saved
     * @param AssetName The base name for texture assets (will be prefixed appropriately)
     * @return True if all textures were created successfully, false otherwise
     */
    UFUNCTION(BlueprintCallable, meta = (Category = "AnimToNiagara"))
    static bool CreateWeightTextureWithLODs(UMPAnimToTextureDataAsset* DataAsset, const FString& PackagePath, const FString& AssetName);

    /**
     * Creates material instances for each LOD and section combination of the static mesh.
     * Generates individual material instances that reference the appropriate weight textures
     * and configures them for use with the animation texture system.
     * 
     * @param DataAsset The data asset containing mesh data and generated textures
     * @param PackagePath The package path where material instances will be saved
     * @param AssetName The base name for material instance assets
     * @return True if all material instances were created successfully, false otherwise
     */
    UFUNCTION(BlueprintCallable, meta = (Category = "AnimToNiagara"))
    static bool CreateMaterialInstanceWithLODs(UMPAnimToTextureDataAsset* DataAsset, const FString& PackagePath, const FString& AssetName);

private:
    /**
     * Creates a persistent 2D texture asset with the specified dimensions and pixel data.
     * Handles package creation, texture configuration, and asset registry registration.
     * 
     * @param PackagePath The package path where the texture will be saved
     * @param AssetName The name for the texture asset
     * @param Width The width of the texture in pixels
     * @param Height The height of the texture in pixels
     * @param PixelData The 16-bit pixel data array for the texture
     * @return The created texture asset, nullptr if creation failed
     */
    static UTexture2D* CreatePersistentTexture(const FString& PackagePath, const FString& AssetName, int32 Width,
                                               int32 Height, const TArray<uint16>& PixelData);
    
    /**
     * Creates a material instance constant asset based on a source material.
     * Sets up the parent material relationship and handles asset creation workflow.
     * 
     * @param PackagePath The package path where the material instance will be saved
     * @param AssetName The name for the material instance asset
     * @param SourceMaterial The base material to use as parent for the instance
     * @return The created material instance, nullptr if creation failed
     */
    static UMaterialInstanceConstant* CreateMaterialInstance(const FString& PackagePath, const FString& AssetName, UMaterialInterface* SourceMaterial);
};