// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MassCommonTypes.h"
#include "MPTriggerVolumeEventData.h"
#include "MPTriggerVolumeEventFragment.generated.h"

/**
 * 
 */
USTRUCT()
struct MAAASSPARTICLE_API FMPTriggerVolumeEventFragment : public FMassFragment
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UMPTriggerVolumeEventData> InteractionDefinition = nullptr;
};