// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MassEntity/Tasks/MPSetAnimStateTask.h"
#include "MassEntity/Fragments/MPAnimStateFragment.h"
#include "MassStateTreeExecutionContext.h"
#include "MassEntityManager.h"
#include "StateTreeLinker.h"
#include "MassBehaviorSettings.h"
#include "MassStateTreeDependency.h"


bool FMPSetAnimStateTask::Link(FStateTreeLinker& Linker)
{
    // Call base implementation to bind instance data
	return Super::Link(Linker);
}

void FMPSetAnimStateTask::GetDependencies(UE::MassBehavior::FStateTreeDependencyBuilder& Builder) const
{
    // We need read-write access to the AnimStateFragment
	Builder.AddReadWrite<FMPAnimStateFragment>();

    // Call base to register any additional dependencies
	Super::GetDependencies(Builder);
}

EStateTreeRunStatus FMPSetAnimStateTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    // Retrieve this task's instance data
    const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

    // Cast to the Mass-specific execution context
    const FMassStateTreeExecutionContext& MassContext = static_cast<const FMassStateTreeExecutionContext&>(Context);

    // Get the entity handle
    const FMassEntityHandle Entity = MassContext.GetEntity();
    if (!Entity.IsValid())
    {
        return EStateTreeRunStatus::Failed;
    }

    // Access the entity manager
    FMassEntityManager& EntityManager = MassContext.GetEntityManager();

    // Get a pointer to the AnimStateFragment
    FMPAnimStateFragment* AnimStateFragment = EntityManager.GetFragmentDataPtr<FMPAnimStateFragment>(Entity);

    if (AnimStateFragment == nullptr)
    {
        return EStateTreeRunStatus::Failed;
    }

    // Write the new animation state
    AnimStateFragment->AnimState = InstanceData.NewAnimState;

    return EStateTreeRunStatus::Succeeded;
}
