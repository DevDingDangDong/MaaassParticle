// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MassCommonTypes.h"
#include "MPTriggerVolumeEventData.h"
#include "MPTriggerVolumeExitTag.generated.h"

// 볼륨 퇴장 상호작용을 처리해달라는 요청 태그
USTRUCT()
struct MAAASSPARTICLE_API FMPTriggerVolumeExitTag : public FMassTag
{
	GENERATED_BODY()
};