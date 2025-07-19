// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPAnimStateSyncProcessor.h"
#include "MPNeedsInitializationTag.h"
#include "MPNiagaraComponentFragment.h"
#include "MPNiagaraParticleIDFragment.h"
#include "MPAnimStateFragment.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "NiagaraDataInterfaceMassEntity.h"
#include "NiagaraSystemInstanceController.h"
#include "MPDeletionTags.h"

/**
 * Constructor implementation.
 */
UMPAnimStateSyncProcessor::UMPAnimStateSyncProcessor()
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Behavior;
	EntityQuery.RegisterWithProcessor(*this);
}

/**
 * Configure the query for entities that have completed initialization.
 */
void UMPAnimStateSyncProcessor::ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager)
{
	EntityQuery.Initialize(EntityManager);
	// Target all Niagara-integrated entities that have finished initialization.
	EntityQuery.AddRequirement<FMPNiagaraParticleIDFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMPNiagaraComponentFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMPAnimStateFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);

	// Exclude entities that have not been initialized.
	EntityQuery.AddTagRequirement<FMPNeedsInitializationTag>(EMassFragmentPresence::None);
}

/**
 * Perform animation state synchronization for each matching entity chunk.
 */
void UMPAnimStateSyncProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(Context, [this](FMassExecutionContext& Context)
		{
			const TConstArrayView<FMPNiagaraParticleIDFragment> ParticleIDFragments = Context.GetFragmentView<FMPNiagaraParticleIDFragment>();
			const TConstArrayView<FMPNiagaraComponentFragment> NiagaraCompFragments = Context.GetFragmentView<FMPNiagaraComponentFragment>();
			const TConstArrayView<FMPAnimStateFragment> AnimStateFragments = Context.GetFragmentView<FMPAnimStateFragment>();
			const TConstArrayView<FTransformFragment> TransformFragments = Context.GetFragmentView<FTransformFragment>();


			for (int32 i = 0; i < Context.GetNumEntities(); ++i)
			{
				const FMassEntityHandle Entity = Context.GetEntity(i);
				UNiagaraComponent* NiagaraComponent = NiagaraCompFragments[i].NiagaraComponent.Get();
				if (!NiagaraComponent)
				{
					Context.Defer().AddTag<FPendingDeletionTag>(Entity);
					continue;
				}
				TArray<UNiagaraDataInterface*> DataInterfaces = NiagaraComponent->GetOverrideParameters().GetDataInterfaces();

				UNiagaraDataInterfaceMassEntity* MassNDI = nullptr;

				for (UNiagaraDataInterface* DI : DataInterfaces)
				{
					MassNDI = Cast<UNiagaraDataInterfaceMassEntity>(DI);
					if (MassNDI)
					{
						break;
					}
				}

				if (MassNDI)
				{
					// Get the system instance controller.
					FNiagaraSystemInstanceControllerConstPtr Controller = NiagaraComponent->GetSystemInstanceController();

					if (!Controller.IsValid())
					{
						Context.Defer().AddTag<FPendingDeletionTag>(Entity);
						continue; // Controller is not valid, skip this entity for this frame.
					}

					FNiagaraSystemInstance* SystemInstance = Controller->GetSystemInstance_Unsafe();

					if (!SystemInstance)
					{
						Context.Defer().AddTag<FPendingDeletionTag>(Entity);
						continue;
					}

					FNDI_MassEntityInstanceData* InstanceData = nullptr;
					
					InstanceData = SystemInstance->FindTypedDataInterfaceInstanceData<FNDI_MassEntityInstanceData>(MassNDI);

					FVector ParticleLocation = TransformFragments[i].GetTransform().GetLocation();
					int ParticleID = ParticleIDFragments[i].NiagaraParticleID;

					if (!InstanceData->MassParticlesData.IsValidId(FSetElementId::FromInteger(ParticleID)) || !InstanceData->MassParticlesData[ParticleID])
					{
						Context.Defer().AddTag<FPendingDeletionTag>(Entity);
						continue;
					}

					FNDI_MassEntityParticleData* ParticleData = InstanceData->MassParticlesData[ParticleID];
					if (!ParticleData)
					{
						continue;
					}

					int AnimState = AnimStateFragments[i].AnimState;

					if (ParticleData->AnimState != AnimState)
					{
						ParticleData->AnimState = AnimState;
					}
				}
			}
		});
}

