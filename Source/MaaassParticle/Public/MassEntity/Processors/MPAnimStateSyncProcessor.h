// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MPAnimStateSyncProcessor.generated.h"

/**
 * Processor that synchronizes the animation state fragment into Niagara particle data.
 *
 * Reads FMPAnimStateFragment and writes its AnimState value into the corresponding
 * UNiagaraDataInterfaceMassEntity instance data for each particle.
 */
UCLASS()
class MAAASSPARTICLE_API UMPAnimStateSyncProcessor : public UMassProcessor
{
	GENERATED_BODY()
	/**
	 * Constructor. Sets execution flags, execution group, and registers the entity query.
	 */
	UMPAnimStateSyncProcessor();

protected:
	/**
	 * Configures the query to select entities that have finished initialization
	 * and contain the required fragments: particle ID, component, anim state, and transform.
	 *
	 * @param EntityManager The MassEntityManager used to initialize the query.
	 */
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	
	/**
	 * Executes the synchronization for each entity chunk.
	 *
	 * Iterates entities, retrieves their animation state fragment and updates
	 * the corresponding MassEntityParticleData AnimState in the Niagara data interface.
	 *
	 * @param EntityManager The MassEntityManager for context.
	 * @param Context       Execution context providing fragment views.
	 */
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	/**
	 * Query selecting entities ready for animation state synchronization.
	 */
	UPROPERTY()
	FMassEntityQuery EntityQuery;

};
