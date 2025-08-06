// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityElementTypes.h"
#include "MPEntityAgeFragment.generated.h"

USTRUCT()
struct FMPEntityAgeFragment : public FMassFragment
{
	GENERATED_BODY()

	float Age = 0.0f;
};
