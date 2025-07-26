// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MPTriggerVolumeEventFragment.h"
#include "MassStateTreeFragments.h" 
#include "MPTriggerVolumeSubsystem.h"
#include "MPVolumeTriggerProcessor.generated.h"

/**
 * Mass processor that checks which trigger volume an entity is currently in.
 *
 * This processor queries all entities with location data, compares their positions
 * against the trigger volume grid, and updates their current volume state.
 * If an entity moves into or out of a volume, it can trigger appropriate interaction events.
 */
UCLASS()
class MAAASSPARTICLE_API UMPVolumeTriggerProcessor : public UMassProcessor
{
	GENERATED_BODY()
    /** Constructor sets execution group and registers entity query */
    UMPVolumeTriggerProcessor();

    /** Configure required fragments and tags for this processor's query */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** Executes the main logic to evaluate entity-volume overlap and dispatch enter/exit logic */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

    /** Query used to collect all relevant entities (usually requires TransformFragment) */
    FMassEntityQuery EntityQuery;

    /** Cached reference to the trigger volume subsystem */
    TObjectPtr<UMPTriggerVolumeSubsystem> VolumeSubsystem;
	
};