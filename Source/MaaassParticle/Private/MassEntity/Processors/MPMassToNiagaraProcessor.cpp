// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MassEntity/Processors/MPMassToNiagaraProcessor.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "MPNiagaraParticleIDFragment.h"
#include "MPNiagaraComponentFragment.h"
#include "MPNeedsInitializationTag.h"
#include "NiagaraDataInterfaceMassEntity.h"
#include "NiagaraSystemInstanceController.h"
#include "MPDeletionTags.h"

UMPMassToNiagaraProcessor::UMPMassToNiagaraProcessor()
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	// After Mass simulation is finished, synchronize the results with the world (Niagara).
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::UpdateWorldFromMass;
	EntityQuery.RegisterWithProcessor(*this);
}

void UMPMassToNiagaraProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.Initialize(EntityManager);
	// Find entities to 'send' data to Niagara.
	EntityQuery.AddRequirement<FMPNiagaraParticleIDFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMPNiagaraComponentFragment>(EMassFragmentAccess::ReadOnly);
	// Transform and Velocity data are read-only.
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	// Exclude uninitialized entities.
	EntityQuery.AddTagRequirement<FMPNeedsInitializationTag>(EMassFragmentPresence::None);
}

void UMPMassToNiagaraProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
		{
			// Get Fragment data as read-only.
			const TConstArrayView<FMPNiagaraParticleIDFragment> ParticleIDFragments = Context.GetFragmentView<FMPNiagaraParticleIDFragment>();
			const TConstArrayView<FMPNiagaraComponentFragment> NiagaraCompFragments = Context.GetFragmentView<FMPNiagaraComponentFragment>();
			const TConstArrayView<FTransformFragment> TransformFragments = Context.GetFragmentView<FTransformFragment>();
			const TConstArrayView<FMassVelocityFragment> VelocityFragments = Context.GetFragmentView<FMassVelocityFragment>();

			for (int32 i = 0; i < Context.GetNumEntities(); ++i)
			{
				const FMassEntityHandle Entity = Context.GetEntity(i);

				UNiagaraComponent* NiagaraComponent = NiagaraCompFragments[i].NiagaraComponent.Get();
				if (!NiagaraComponent)
				{
					Context.Defer().AddTag<FPendingDeletionTag>(Entity);
					continue;
				}

				// Finding UMassNiagaraDataInterface. This process is the same.
				UNiagaraDataInterfaceMassEntity* MassNDI = nullptr;
				TArray<UNiagaraDataInterface*> DataInterfaces = NiagaraComponent->GetOverrideParameters().GetDataInterfaces();
				for (UNiagaraDataInterface* DI : DataInterfaces)
				{
					MassNDI = Cast<UNiagaraDataInterfaceMassEntity>(DI);
					if (MassNDI)
					{
						break;
					}
				}

				if (!MassNDI)
				{
					Context.Defer().AddTag<FPendingDeletionTag>(Entity);
					continue;
				}
				
				// The process of obtaining the system instance controller and data is also the same.
				FNiagaraSystemInstanceControllerConstPtr Controller = NiagaraComponent->GetSystemInstanceController();

				if (!Controller.IsValid())
				{
					Context.Defer().AddTag<FPendingDeletionTag>(Entity);
					continue;
				}

				FNiagaraSystemInstance* SystemInstance = Controller->GetSystemInstance_Unsafe();
				if (!SystemInstance)
				{
					Context.Defer().AddTag<FPendingDeletionTag>(Entity);
					continue;
				}
				
				FNDI_MassEntityInstanceData* InstanceData = SystemInstance->FindTypedDataInterfaceInstanceData<FNDI_MassEntityInstanceData>(MassNDI);
				if (!InstanceData)
				{
					Context.Defer().AddTag<FPendingDeletionTag>(Entity);
					continue;
				}

				if (FNDI_MassEntityParticleData* ParticleData = MassNDI->GetParticleData(InstanceData, ParticleIDFragments[i].NiagaraParticleID))
				{
					ParticleData->Position = (FVector3f)TransformFragments[i].GetTransform().GetLocation(); // Casting to FVector3f is recommended
					ParticleData->Orientation = TransformFragments[i].GetTransform().GetRotation();
					ParticleData->Velocity = (FVector3f)VelocityFragments[i].Value; // Casting to FVector3f is recommended
				}
			}
		});
}
