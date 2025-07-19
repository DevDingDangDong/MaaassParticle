// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AnimToTexture/MPAnimToTextureDataAsset.h"
#include "NiagaraSystem.h"
#include "MassEntityConfigAsset.h"
#include "MPSpawnerDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MAAASSPARTICLE_API UMPSpawnerDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
    /** AnimToTexture Data Asset for driving animation-to-texture conversions */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EasyCrowd")
    UMPAnimToTextureDataAsset* AnimToTextureDataAsset;

    ///** Niagara system for crowd particle effects */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EasyCrowd")
    UNiagaraSystem* CrowdNiagaraSystem;

    /** Mass Entity Config Asset for configuring entity behaviors */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EasyCrowd")
    UMassEntityConfigAsset* EntityConfigAsset;

};
