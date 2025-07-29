// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MPGroundFollowingProcessor.generated.h"

/**
 * @class UMassStickToGroundProcessor
 * @brief A Mass processor that adjusts an entity's Z-location to stick to the ground.
 * It queries for entities with the FMassFollowGroundTag and uses settings from
 * FMassGroundTraceSettingsFragment to perform a line trace downwards to find the ground.
 */
UCLASS()
class MAAASSPARTICLE_API UMPGroundFollowingProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
    UMPGroundFollowingProcessor();

protected:
    /** Overridden to configure the entity query. */
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;

    /** Overridden to execute the processor's logic on the queried entities. */
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
	/**
	 * Query selecting entities ready for animation state synchronization.
	 */
	UPROPERTY()
	FMassEntityQuery EntityQuery;
};
