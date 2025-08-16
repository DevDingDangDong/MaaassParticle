// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MPMassToNiagaraProcessor.generated.h"

/**
 * Processor that synchronizes Mass entity data into the Niagara particle system.
 *
 * Reads FMPNiagaraParticleIDFragment, FMPNiagaraComponentFragment, FTransformFragment,
 * and FMassVelocityFragment from each entity, then writes position, orientation,
 * and velocity into the corresponding UNiagaraDataInterfaceMassEntity for that particle.
 */
UCLASS()
class MAAASSPARTICLE_API UMPMassToNiagaraProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	/**
	 * Constructor. Sets execution flags and registers the entity query.
	 */
	UMPMassToNiagaraProcessor();

protected:
	/**
	 * Configures the entity query to select entities with the required fragments and tags.
	 *
	 * @param EntityManager The Mass Entity Manager used to initialize the query.
	 */
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	
	/**
	 * Executes the processor: iterates over matching entities and writes Mass data into Niagara.
	 *
	 * @param EntityManager The Mass Entity Manager for accessing fragments.
	 * @param Context       The execution context providing fragment views and chunk iteration.
	 */
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	UPROPERTY()
	FMassEntityQuery EntityQuery;

};
