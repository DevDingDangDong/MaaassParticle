// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MPInitializerProcessor.generated.h"

/**
 * Processor that initializes newly spawned Mass entities from Niagara data.
 *
 * Selects entities tagged for initialization, reads initial particle state (position, velocity, anim state)
 * from the Niagara MassEntity data interface, applies it to transform, velocity, and anim state fragments,
 * and removes the initialization tag so this processor runs only once per entity.
 */
UCLASS()
class MAAASSPARTICLE_API UMPInitializerProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	/**
	 * Constructor. Sets execution flags, execution group, and registers the entity query.
	 */
	UMPInitializerProcessor();

protected:
	/**
	 * Configures the entity query to select entities that require initialization.
	 *
	 * Requires:
	 * - FECNiagaraParticleIDFragment (read-only)
	 * - FECNiagaraComponentFragment (read-only)
	 * - FTransformFragment (read-write)
	 * - FMassVelocityFragment (read-write)
	 * - FECAnimStateFragment (read-write)
	 * Tag requirement:
	 * - FECNeedsInitializationTag must be present.
	 *
	 * @param EntityManager The MassEntityManager used to initialize the query.
	 */
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	
	/**
	 * Executes initialization on each entity chunk:
	 * - Reads particle ID, component, transform, velocity, and anim state fragments.
	 * - Fetches initial spawn data from UNiagaraDataInterfaceMassEntity.
	 * - Writes position, velocity, and anim state into the corresponding fragments.
	 * - Removes the FECNeedsInitializationTag to prevent re-execution.
	 *
	 * @param EntityManager The MassEntityManager for adding/removing tags.
	 * @param Context       The execution context providing fragment views and deferred operations.
	 */
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	/** Query that selects entities tagged for initialization. */
	UPROPERTY()
	FMassEntityQuery EntityQuery;
};
