// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPRequestEventTask.h"
#include "MassStateTreeFragments.h"
#include "MassAgentSubsystem.h"
#include "MPTriggerVolumeRequestEventFragment.h"

void UMPRequestEventTask::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context, const FMassEntityHandle Entity) const
{
	Super::Execute(EntityManager, Context, Entity);

	// Ensure the event tag is valid before proceeding
	if (!EventToSend.Tag.IsValid())
	{
		return;
	}

	// Retrieve the fragment that stores pending StateTree events for this entity
	FMPTriggerVolumeRequestEventFragment* RequestFrag = EntityManager.GetFragmentDataPtr<FMPTriggerVolumeRequestEventFragment>(Entity);

	// If the fragment exists, queue the event for processing
	if (RequestFrag)
	{
		RequestFrag->PendingEvents.Add(EventToSend);
	}
}
