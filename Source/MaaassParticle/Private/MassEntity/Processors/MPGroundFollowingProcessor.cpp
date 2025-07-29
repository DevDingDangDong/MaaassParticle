// Copyright 2025 DevDingDangDong, All Rights Reserved.


#include "MassEntity/Processors/MPGroundFollowingProcessor.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassCommonFragments.h"
#include "MPGroundTraceFragment.h"
#include "Movement/MassMovementProcessors.h"


UMPGroundFollowingProcessor::UMPGroundFollowingProcessor()
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
	ExecutionOrder.ExecuteAfter.Add(UMassApplyMovementProcessor::StaticClass()->GetFName());
	EntityQuery.RegisterWithProcessor(*this);
}

void UMPGroundFollowingProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	// Configure the query: requires a mutable Transform, a read-only GroundTraceSettings, and the FollowGroundTag.
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMPGroundTraceFragment>(EMassFragmentAccess::ReadOnly); // Settings are only read.
}

void UMPGroundFollowingProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{

    EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& Context)
        {
            const int32 NumEntities = Context.GetNumEntities();
            const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
            const TConstArrayView<FMPGroundTraceFragment> SettingsList = Context.GetFragmentView<FMPGroundTraceFragment>();

            UWorld* World = GetWorld();
            if (!World)
            {
                return;
            }

            for (int32 i = 0; i < NumEntities; ++i)
            {
                FTransform& Transform = TransformList[i].GetMutableTransform();
                const FMPGroundTraceFragment& Settings = SettingsList[i];

                FVector CurrentLocation = Transform.GetLocation();

                const FVector TraceStart = CurrentLocation + FVector(0, 0, Settings.TraceStartUpOffset);
                const FVector TraceEnd = CurrentLocation - FVector(0, 0, Settings.TraceEndDownOffset);

                FHitResult HitResult;
                const bool bHit = World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility);

                if (bHit && HitResult.ImpactPoint.Z < TraceStart.Z)
                {
                    CurrentLocation.Z = HitResult.ImpactPoint.Z + Settings.GroundHeightOffset;
                    Transform.SetLocation(CurrentLocation);
                }
            }
        });
}
