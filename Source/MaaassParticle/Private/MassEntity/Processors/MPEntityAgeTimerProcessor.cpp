// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPEntityAgeTimerProcessor.h"

#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MPEntityAgeFragment.h"

UMPEntityAgeTimerProcessor::UMPEntityAgeTimerProcessor()
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::SyncWorldToMass;
	EntityQuery.RegisterWithProcessor(*this);
}

void UMPEntityAgeTimerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	// Configure the query: requires a mutable Transform, a read-only GroundTraceSettings, and the FollowGroundTag.
	EntityQuery.AddRequirement<FMPEntityAgeFragment>(EMassFragmentAccess::ReadWrite);
}

void UMPEntityAgeTimerProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{

    EntityQuery.ForEachEntityChunk( Context, [this](FMassExecutionContext& Context)
        {
            const int32 NumEntities = Context.GetNumEntities();
            const TArrayView<FMPEntityAgeFragment> AgeList = Context.GetMutableFragmentView<FMPEntityAgeFragment>();

            for (int32 i = 0; i < NumEntities; ++i)
            {
            	AgeList[i].Age += Context.GetDeltaTimeSeconds();
            }
        });
}
