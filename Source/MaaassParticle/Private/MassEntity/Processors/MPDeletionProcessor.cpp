// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPDeletionProcessor.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MPDeletionTags.h"

/**
 * Constructor implementation.
 */
UMPDeletionProcessor::UMPDeletionProcessor()
{
	// Ensure this processor runs after the Behavior group.
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Behavior);
	EntityQuery.RegisterWithProcessor(*this);
}

/**
 * Configure the entity query to select entities pending deletion.
 */
void UMPDeletionProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.Initialize(EntityManager);
	EntityQuery.AddTagRequirement<FPendingDeletionTag>(EMassFragmentPresence::All);
}

/**
 * Execute deletion for each frame:
 * - Collect up to MaxEntitiesToDestroyPerFrame entities.
 * - Defer their destruction to the context.
 */
void UMPDeletionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	// Declare an array to collect the handles of entities to be destroyed.
	TArray<FMassEntityHandle> EntitiesToDestroyThisFrame;
	EntitiesToDestroyThisFrame.Reserve(MaxEntitiesToDestroyPerFrame);

	// Iterate through the entities to be destroyed.
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, &EntitiesToDestroyThisFrame](FMassExecutionContext& ChunkContext)
	{
		const TConstArrayView<FMassEntityHandle> EntitiesInChunk = ChunkContext.GetEntities();
		for (const FMassEntityHandle& Entity : EntitiesInChunk)
		{
			if (EntitiesToDestroyThisFrame.Num() >= MaxEntitiesToDestroyPerFrame)
			{
				return;
			}
			EntitiesToDestroyThisFrame.Add(Entity);
		}
	});

	if (EntitiesToDestroyThisFrame.Num() > 0)
	{
		Context.Defer().DestroyEntities(EntitiesToDestroyThisFrame);
	}
}
