// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MPEntiySyncTag.generated.h"

/**
 * Tag indicating that the entity requires initialization from Niagara data.
 *
 * Entities with this tag will be processed by the initializer processor once.
 */
USTRUCT()
struct MAAASSPARTICLE_API FMPEntitySyncTag : public FMassTag
{
	GENERATED_BODY()
};