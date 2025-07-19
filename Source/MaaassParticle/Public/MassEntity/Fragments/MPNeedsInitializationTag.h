// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MPNeedsInitializationTag.generated.h"

/**
 * Tag indicating that the entity requires initialization from Niagara data.
 *
 * Entities with this tag will be processed by the initializer processor once.
 */
USTRUCT()
struct MAAASSPARTICLE_API FMPNeedsInitializationTag : public FMassTag
{
	GENERATED_BODY()
	
};