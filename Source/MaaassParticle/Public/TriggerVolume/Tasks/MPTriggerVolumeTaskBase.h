// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "StateTreeEvents.h"
#include "MassEntityHandle.h"
#include "MassExecutionContext.h"
#include "MPTriggerVolumeTaskBase.generated.h"

/**
 * Base class for custom trigger volume tasks.
 *
 * Inherit from this class to define actions that should be executed when an entity
 * enters or exits a trigger volume. These tasks can be assigned through data assets
 * and executed via interaction processors.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories)
class MAAASSPARTICLE_API UMPTriggerVolumeTaskBase : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Execute the logic associated with this task.
	 *
	 * @param EntityManager The entity manager to access or modify entity data.
	 * @param Context Execution context containing runtime info for this tick.
	 * @param Entity The specific entity to apply the task to.
	 */
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context, const FMassEntityHandle Entity) const
	{
		// Override this method in derived classes to define custom behavior.
	}
};