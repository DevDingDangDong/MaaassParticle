// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MassEntity/Processors/TriggerVolume/MPVolumeTriggerProcessor.h"
#include "MassCommonFragments.h"
#include "MPTriggerVolumeComponent.h"
#include "MPTriggerVolumeRequestEventFragment.h"
#include "MassSignalSubsystem.h" 
#include "MassSignalTypes.h"

UMPVolumeTriggerProcessor::UMPVolumeTriggerProcessor()
{
    ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::SyncWorldToMass;
    bAutoRegisterWithProcessingPhases = true;
    EntityQuery.RegisterWithProcessor(*this);
}

void UMPVolumeTriggerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    EntityQuery.Initialize(EntityManager);
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FMPTriggerVolumeEventFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FMassStateTreeInstanceFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FMPTriggerVolumeRequestEventFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddConstSharedRequirement<FMassStateTreeSharedFragment>(EMassFragmentPresence::All);
    EntityQuery.RegisterWithProcessor(*this);
}

void UMPVolumeTriggerProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    UWorld* World = EntityManager.GetWorld();

    if (!World)
    {
        return;
    }

    if (VolumeSubsystem == nullptr)
    {
        VolumeSubsystem = World->GetSubsystem<UMPTriggerVolumeSubsystem>();
        if (!VolumeSubsystem) return;
    }
    

    UMassStateTreeSubsystem* StateTreeSubsystem = World->GetSubsystem<UMassStateTreeSubsystem>();
    if (!StateTreeSubsystem)
    {
        return;
    }

    UMassSignalSubsystem* SignalSubsystem = World->GetSubsystem<UMassSignalSubsystem>();
    if (!SignalSubsystem)
    {
        return;
    }
    EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, &EntityManager, StateTreeSubsystem, SignalSubsystem](FMassExecutionContext& ChunkContext)
    {
        const TConstArrayView<FTransformFragment> LocationList = ChunkContext.GetFragmentView<FTransformFragment>();
        const TArrayView<FMPTriggerVolumeEventFragment> InteractionFragmentList = ChunkContext.GetMutableFragmentView<FMPTriggerVolumeEventFragment>();
        const TConstArrayView<FMassStateTreeInstanceFragment> InstanceFrags = ChunkContext.GetFragmentView<FMassStateTreeInstanceFragment>();
        const FMassStateTreeSharedFragment& SharedFrag = ChunkContext.GetConstSharedFragment<FMassStateTreeSharedFragment>();

        for (int32 EntityIndex = 0; EntityIndex < ChunkContext.GetNumEntities(); ++EntityIndex)
        {
            const FName ForceStateTreeEvalSignal = FName(TEXT("ForceStateTreeEval"));
            const FMassEntityHandle Entity = ChunkContext.GetEntity(EntityIndex);
            const FVector& Location = LocationList[EntityIndex].GetTransform().GetLocation();
            const FMassStateTreeInstanceHandle& InstanceHandle = InstanceFrags[EntityIndex].InstanceHandle;
            const UStateTree* StateTreeAsset = SharedFrag.StateTree;
            const TArrayView<FMPTriggerVolumeRequestEventFragment> RequestFrags = ChunkContext.GetMutableFragmentView<FMPTriggerVolumeRequestEventFragment>();

            TWeakObjectPtr<UMPTriggerVolumeComponent> PrevVolume = VolumeSubsystem->GetVolumeForEntity(Entity);
            UMPTriggerVolumeComponent* CurrentVolume = VolumeSubsystem->FindBestInteractionSourceAtLocation(Location);

            // Detect a volume change
            if (PrevVolume.Get() != CurrentVolume)
            {
                // 1. Handle exit logic from previous volume
                if (PrevVolume.IsValid() && PrevVolume->ExitInteraction)
                {
                    for (const TObjectPtr<UMPTriggerVolumeTaskBase>& Task : PrevVolume->ExitInteraction->Tasks)
                    {
                        if (Task)
                        {
                            // Each task queues its desired events via fragment
                            Task->Execute(EntityManager, ChunkContext, Entity);
                        }
                    }
                }

                // 2. Handle enter logic for new volume
                if (CurrentVolume && CurrentVolume->EnterInteraction)
                {
                    for (const TObjectPtr<UMPTriggerVolumeTaskBase>& Task : CurrentVolume->EnterInteraction->Tasks)
                    {
                        if (Task)
                        {
                            Task->Execute(EntityManager, ChunkContext, Entity);
                        }
                    }
                }

                // 3. Send queued events to the StateTree
                FMPTriggerVolumeRequestEventFragment& EventFragment = RequestFrags[EntityIndex];

                if (!EventFragment.PendingEvents.IsEmpty())
                {
                    if (InstanceHandle.IsValid() && StateTreeAsset)
                    {
                        const FName WakeUpSignal = FName(TEXT("MPStateTree.WakeUp"));
                        SignalSubsystem->SignalEntity(WakeUpSignal, Entity);
                        if (FStateTreeInstanceData* InstanceData = StateTreeSubsystem->GetInstanceData(InstanceHandle))
                        {
                            FStateTreeMinimalExecutionContext StateTreeContext(*StateTreeSubsystem, *StateTreeAsset, *InstanceData);

                            const double SendTime = FPlatformTime::Seconds();
                            for (const FStateTreeEvent& Event : EventFragment.PendingEvents)
                            {
                                StateTreeContext.SendEvent(Event.Tag, Event.Payload, NAME_None);
                                UE_LOG(LogTemp, Warning, TEXT("[SendEvent] Sent Event: %s → Entity[%d:%d]"),
                                    *Event.Tag.ToString(), Entity.Index, Entity.SerialNumber);
                               SignalSubsystem->SignalEntities(UE::Mass::Signals::StateTreeActivate, { Entity });
                                
                            }
                        }
                        else {
                            continue;
                        }
                    }

                    // 4. Clear the event queue after sending
                    EventFragment.PendingEvents.Reset();
                }

                // 5. Update the current volume assignment in the subsystem
                VolumeSubsystem->UpdateEntityCurrentVolume(Entity, CurrentVolume);
            }
        }
    });
}
