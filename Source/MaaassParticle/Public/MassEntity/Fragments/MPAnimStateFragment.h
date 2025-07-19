// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityElementTypes.h"
#include "MPAnimStateFragment.generated.h"

/**
 * Stores the current animation state index for a Mass entity.
 *
 * Used by state sync and set-state tasks to read/write the entity's anim state.
 */
USTRUCT()
struct FMPAnimStateFragment : public FMassFragment
{
	GENERATED_BODY()

	/** The current animation state value. */
	int AnimState = -1;
};
