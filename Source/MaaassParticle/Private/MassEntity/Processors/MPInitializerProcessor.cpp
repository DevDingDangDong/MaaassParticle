// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPInitializerProcessor.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MPNiagaraComponentFragment.h"
#include "MPNiagaraParticleIDFragment.h"
#include "NiagaraDataInterfaceMassEntity.h"
#include "MPAnimStateFragment.h"
#include <MassMovementFragments.h>
#include <MPNeedsInitializationTag.h>
#include "NiagaraSystemInstanceController.h"


/**
 * Constructor implementation.
 */
UMPInitializerProcessor::UMPInitializerProcessor()
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Behavior;
    EntityQuery.RegisterWithProcessor(*this);
}

/**
 * Configure the entity query requirements.
 */
void UMPInitializerProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
    EntityQuery.Initialize(EntityManager);
    EntityQuery.AddRequirement<FMPNiagaraParticleIDFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FMPNiagaraComponentFragment>(EMassFragmentAccess::ReadOnly);
    EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddRequirement<FMPAnimStateFragment>(EMassFragmentAccess::ReadWrite);
    EntityQuery.AddTagRequirement<FMPNeedsInitializationTag>(EMassFragmentPresence::All);
}

/**
 * Perform initialization for each matching entity chunk.
 */
void UMPInitializerProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
    EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
    {
        const TConstArrayView<FMPNiagaraParticleIDFragment> ParticleIDFragments = Context.GetFragmentView<FMPNiagaraParticleIDFragment>();
        const TConstArrayView<FMPNiagaraComponentFragment> NiagaraCompFragments = Context.GetFragmentView<FMPNiagaraComponentFragment>();
        const TArrayView<FTransformFragment> TransformFragments = Context.GetMutableFragmentView<FTransformFragment>();
        const TArrayView<FMassVelocityFragment> VelocityFragments = Context.GetMutableFragmentView<FMassVelocityFragment>();
        const TArrayView<FMPAnimStateFragment> AnimStateFragments = Context.GetMutableFragmentView<FMPAnimStateFragment>();

        const int32 NumEntities = Context.GetNumEntities();

        for (int32 i = 0; i < NumEntities;i++)
        {
            const FMassEntityHandle Entity = Context.GetEntity(i);
            const FMPNiagaraParticleIDFragment& ParticleIDFragment = ParticleIDFragments[i];
            const FMPNiagaraComponentFragment& NiagaraCompFragment = NiagaraCompFragments[i];
            FTransformFragment& TransformFragment = TransformFragments[i];
            FMassVelocityFragment& VelocityFragment = VelocityFragments[i];
            FMPAnimStateFragment& AnimStateFragment = AnimStateFragments[i];

            UNiagaraComponent* NiagaraComponent = NiagaraCompFragment.NiagaraComponent.Get();

            if (!NiagaraComponent)
            {
                // If the component is invalid, remove the initialization tag and continue.
                Context.Defer().RemoveTag<FMPNeedsInitializationTag>(Context.GetEntity(i));
            }
            
            TArray<UNiagaraDataInterface*> DataInterfaces = NiagaraComponent->GetOverrideParameters().GetDataInterfaces();

            UNiagaraDataInterfaceMassEntity* MassNDI = nullptr;


            for (UNiagaraDataInterface* DI : DataInterfaces)
            {
                MassNDI = Cast<UNiagaraDataInterfaceMassEntity>(DI);
                if (MassNDI)
                {
                    break; // If found, break the loop.
                }
            }

            if (MassNDI)
            {
                // Get the system instance controller.
                FNiagaraSystemInstanceControllerConstPtr Controller = NiagaraComponent->GetSystemInstanceController();
                FNDI_MassEntityInstanceData* InstanceData = nullptr;
                if (Controller.IsValid() && MassNDI)
                {
                    FNiagaraSystemInstance* SystemInstance = Controller->GetSystemInstance_Unsafe();

                    SystemInstance->GetId();
                    InstanceData = SystemInstance->FindTypedDataInterfaceInstanceData<FNDI_MassEntityInstanceData>(MassNDI);
                }
                if (const FNDI_MassEntityParticleData* ParticleData = MassNDI->GetParticleData(InstanceData, ParticleIDFragment.NiagaraParticleID))
                {
                    // Apply position
                    FVector NewLocation;
                    NewLocation.X = ParticleData->Position.X;
                    NewLocation.Y = ParticleData->Position.Y;
                    NewLocation.Z = ParticleData->Position.Z;

                    // Apply velocity
                    FVector NewVelocity;
                    NewVelocity.X = ParticleData->Velocity.X;
                    NewVelocity.Y = ParticleData->Velocity.Y;
                    NewVelocity.Z = ParticleData->Velocity.Z;
                    
                    VelocityFragment.Value = NewVelocity;

                    // Apply animation state
                    AnimStateFragment.AnimState = ParticleData->AnimState;
                }
            }

            // Remove the initialization tag to prevent this processor from running again.
            Context.Defer().RemoveTag<FMPNeedsInitializationTag>(Context.GetEntity(i));
        }
    });
}
