// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassStateTreeTypes.h"
#include "MPSetAnimStateTask.generated.h"

/**
 * Instance data for FECSetAnimStateTask.
 *
 * Holds the new animation state value that will be applied to the entity.
 */
USTRUCT()
struct FMPSetAnimStateTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Animation")
	int32 NewAnimState = 0;
};


/**
 * StateTree task that sets the animation state fragment for an entity.
 *
 * This task reads the NewAnimState value from its instance data and writes it into
 * the entity's FECAnimStateFragment when entering the state.
 */
USTRUCT(meta = (DisplayName = "MP Set Anim State"))
struct MAAASSPARTICLE_API FMPSetAnimStateTask : public FMassStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMPSetAnimStateTaskInstanceData;

protected:
	/**
	 * Returns the struct type used for this task's instance data.
	 *
	 * @return The UStruct of FECSetAnimStateTaskInstanceData.
	 */
    virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/**
	 * Links this task into the StateTree, binding its instance data.
	 *
	 * @param Linker The linker used to bind instance data and parameters.
	 * @return true if linking succeeded, false otherwise.
	 */
    virtual bool Link(FStateTreeLinker& Linker) override;

	/**
	 * Declares fragment access dependencies for this task.
	 *
	 * Adds read-write access to FECAnimStateFragment since we modify it.
	 *
	 * @param Builder Dependency builder to register fragment access.
	 */
    virtual void GetDependencies(UE::MassBehavior::FStateTreeDependencyBuilder& Builder) const override;

	/**
	 * Called when entering the state to perform the animation state update.
	 *
	 * @param Context    Execution context for the state tree.
	 * @param Transition The transition that triggered this state entry.
	 * @return EStateTreeRunStatus::Succeeded if the fragment was updated; otherwise Failed.
	 */
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
