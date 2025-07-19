// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MPDeletionProcessor.generated.h"

/**
 * Processor that destroys Mass entities marked for deletion.
 *
 * Selects entities tagged with FPendingDeletionTag, up to a configurable maximum
 * per frame, and defers their destruction to avoid immediate deletion during simulation.
 */

UCLASS()
class MAAASSPARTICLE_API UMPDeletionProcessor : public UMassProcessor
{
	GENERATED_BODY()

protected:
	/**
	 * Constructor. Sets the execution order to run after the Behavior group
	 * and registers the entity query for deletion.
	 */
	UMPDeletionProcessor();

	/**
	 * Configures the query to select entities with the pending deletion tag.
	 *
	 * @param EntityManager The MassEntityManager used to initialize the query.
	 */
	virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
	
	/**
	 * Executes deletion: gathers up to MaxEntitiesToDestroyPerFrame entity handles
	 * and defers their destruction via the context.
	 *
	 * @param EntityManager The MassEntityManager (not used directly here).
	 * @param Context The execution context used to defer entity destruction.
	 */
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	/** Query that selects entities tagged for pending deletion. */
	UPROPERTY()
	FMassEntityQuery EntityQuery;

protected:
	/** Maximum number of entities to destroy per frame. */
	UPROPERTY(EditDefaultsOnly, Category = "Deletion")
	int32 MaxEntitiesToDestroyPerFrame = 200;
	
};

namespace UE::EC::ProcessorGroupNames
{
	// Define a new group name to be used exclusively for destruction.
	const FName Deletion = FName(TEXT("Deletion"));
}