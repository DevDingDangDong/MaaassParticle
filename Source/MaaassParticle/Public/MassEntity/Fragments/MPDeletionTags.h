// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MPDeletionTags.generated.h"

/**
 * Tag marking entities pending deletion by the deletion processor.
 */
USTRUCT()
struct MAAASSPARTICLE_API FPendingDeletionTag : public FMassTag
{
	GENERATED_BODY()
};