// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "StateTreeEvents.h"
#include "MassEntityHandle.h"
#include "MassExecutionContext.h"
#include "MPTriggerVolumeTaskBase.h"
#include "MPRequestEventTask.generated.h"

/**
 * Task that sends a StateTree event to a specified entity.
 *
 * This task is typically used when an entity enters or exits a trigger volume,
 * and a specific event needs to be sent to trigger a transition in its StateTree.
 */
UCLASS()
class MAAASSPARTICLE_API UMPRequestEventTask : public UMPTriggerVolumeTaskBase
{
	GENERATED_BODY()

public:
	/**
	 * Executes the task: queues a StateTree event to be sent to the entity.
	 *
	 * @param EntityManager The manager that owns and tracks all entities.
	 * @param Context The execution context for the current processing tick.
	 * @param Entity The entity to which the event will be sent.
	 */
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context, const FMassEntityHandle Entity) const override;
protected:
	/**
	 * The event to send to the entity's StateTree.
	 * This should match a StateTree transition trigger tag.
	 */
	UPROPERTY(EditAnywhere, Category = "Event")
	FStateTreeEvent EventToSend;
};

