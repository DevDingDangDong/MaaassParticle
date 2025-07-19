// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "AnimToTexture/Public/AnimToTextureDataAsset.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MPAnimToTextureDataAsset.generated.h"

/**
 * Holds per-LOD configuration for the AnimToTexture system.
 *
 * Contains an array of material instances to apply at this LOD
 * and the corresponding generated texture when in Bone mode.
 */
USTRUCT(BlueprintType)
struct FMPAnimToTextureLODInfo
{
	GENERATED_BODY()

	/**
	 * Material instances corresponding to mesh sections at this LOD.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD", meta = (EditCondition = "Mode == EAnimToTextureMode::Bone", EditConditionHides))
	TArray<UMaterialInstanceConstant*> MaterialInstances;
	
	/**
	 * Texture generated for this LOD when using Bone mode.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD", meta = (EditCondition = "Mode == EAnimToTextureMode::Bone", EditConditionHides))
	UTexture2D* Texture;
};

/**
 * Data asset extending UAnimToTextureDataAsset with per-LOD info.
 *
 * Provides an array of FEMPAnimToTextureLODInfo entries to configure
 * material and texture data for each LOD when in Bone mode.
 */
UCLASS()
class UMPAnimToTextureDataAsset : public UAnimToTextureDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Array of per-LOD settings, only editable in Bone mode.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD", meta = (EditCondition = "Mode == EAnimToTextureMode::Bone", EditConditionHides))
	TArray<FMPAnimToTextureLODInfo> LODInfos;
};
