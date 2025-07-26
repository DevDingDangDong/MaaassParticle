// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MassCommonTypes.h"
#include "MPTriggerVolumeEventData.h"
#include "MPTriggerVolumeRequestEventFragment.generated.h"

USTRUCT()
struct MAAASSPARTICLE_API FMPTriggerVolumeRequestEventFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FStateTreeEvent> PendingEvents;
};