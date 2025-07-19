// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MPWidgetLibrary.generated.h"

class UMPAnimToTextureDataAsset;
/**
 * 
 */
UCLASS()
class UMPWidgetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

    /*
     * @brief Creates, populates, and saves a new UEMPAnimToTextureDataAsset.
     * @details This editor-only utility function streamlines the creation of a data asset. It performs validation on all inputs,
     * constructs a unique asset name (e.g., DA_AssetName_BAT), creates the asset in the specified path,
     * populates it with the provided skeletal mesh and animations, and saves it to disk.
     * The function will fail if an asset with the same name already exists in the target path.
     *
     * @param PackagePath The content browser path where the new data asset will be created (e.g., /Game/MyFolder). Must be a valid content path.
     * @param AssetName The base name for the new data asset. A standard prefix and suffix will be added automatically.
     * @param TextureMaxWidth  The maximum width for the generated animation texture.
     * @param TextureMaxHeight The maximum height for the generated animation texture.
     * @param SkeletalMesh The skeletal mesh to be assigned to the new data asset.
     * @param SelectedAnims An array of animation sequences to be added to the new data asset.
     * @return A pointer to the newly created and saved UEMPAnimToTextureDataAsset on success, or nullptr if any part of the process fails.
     */
    UFUNCTION(BlueprintCallable, Category = "Easy Crowd Widget")
    static UMPAnimToTextureDataAsset* CreateAndProcessDataAsset_Library(
        const FString& PackagePath,
        const FString& AssetName,
        int32 TextureMaxWidth,
		int32 TextureMaxHeight,
        USkeletalMesh* SkeletalMesh,
        const TArray<UAnimSequence*>& SelectedAnims
    );

    /*
     * @brief Copies a given FString to the system clipboard.
     * @details This is a simple helper function that uses the engine's platform-agnostic functionality
     * to copy a string to the user's operating system clipboard.
     *
     * @param TextToCopy The string that will be copied to the clipboard.
     */
    UFUNCTION(BlueprintCallable, Category = "Easy Crowd Widget")
    static void CopyTextToClipboard(const FString& TextToCopy);
};
