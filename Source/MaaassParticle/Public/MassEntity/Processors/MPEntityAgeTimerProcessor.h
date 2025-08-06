// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MPEntityAgeTimerProcessor.generated.h"

UCLASS()
class UMPEntityAgeTimerProcessor : public UMassProcessor
{
public:
	GENERATED_BODY()
	
public:
	UMPEntityAgeTimerProcessor();

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
