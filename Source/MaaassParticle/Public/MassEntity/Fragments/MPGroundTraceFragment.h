// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityElementTypes.h"
#include "MPGroundTraceFragment.generated.h"

/**
 * @struct FMassGroundTraceSettingsFragment
 * @brief A fragment that holds settings for the ground detection line trace.
 * Entities with this fragment will be targeted by the ground-following logic.
 */
USTRUCT()
struct FMPGroundTraceFragment : public FMassFragment
{
	GENERATED_BODY()

	/**
	 * @brief The height offset to add to the entity's current location to determine the start of the line trace.
	 * This should be large enough to be above any potential terrain height changes.
	 * @default 1000.0f
	 */
	UPROPERTY(EditAnywhere, Category = "Ground Following")
	float TraceStartUpOffset = 1000.0f;

	/**
	 * @brief The distance to trace downwards from the entity's current location to determine the end of the line trace.
	 * This should be large enough to account for any potential drops in terrain.
	 * @default 2000.0f
	 */
	UPROPERTY(EditAnywhere, Category = "Ground Following")
	float TraceEndDownOffset = 2000.0f;

	/**
	 * @brief The height offset to add to the hit impact point when the ground is successfully found.
	 * Adjust this to match the entity's feet location or pivot.
	 * @default 5.0f
	 */
	UPROPERTY(EditAnywhere, Category = "Ground Following")
	float GroundHeightOffset = 0.0f;
};
