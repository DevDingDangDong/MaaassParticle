// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AnimToTexture/MPAnimToTextureDataAsset.h"
#include "NiagaraSystem.h"
#include "MassEntityConfigAsset.h"
#include "Niagara/Internal/NiagaraSystemEmitterState.h"
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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle")
    TSoftObjectPtr<UMPAnimToTextureDataAsset> AnimToTextureDataAsset;

    ///** Niagara system for crowd particle effects */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle")
    TSoftObjectPtr<UNiagaraSystem> CrowdNiagaraSystem;

    /** Mass Entity Config Asset for configuring entity behaviors */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle")
    TSoftObjectPtr<UMassEntityConfigAsset> EntityConfigAsset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle")
    int32 DefaultAnimState;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle")
    ENiagaraLoopBehavior LoopBehavior = ENiagaraLoopBehavior::Once;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle", meta = (EditCondition = "LoopBehavior == ENiagaraLoopBehavior::Multiple", ClampMin = "1"))
    int32 LoopCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle", meta = (EditCondition = "LoopBehavior != ENiagaraLoopBehavior::Once"))
    float LoopDuration = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle")
    bool KillParticleOnLifeHasElapsed = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle", meta = (EditCondition = "KillParticleOnLifeHasElapsed == true"))
    float ParticleLifeTime = 1.0f;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle")
    FMassSpawnDataGenerator SpawnDataGenerator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle")
    int32 SpawnCount;
};
