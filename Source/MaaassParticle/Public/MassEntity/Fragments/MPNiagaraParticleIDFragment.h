// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MassCommonFragments.h"
#include "NiagaraComponent.h"
#include "MPNiagaraParticleIDFragment.generated.h"

/**
 * Stores the ID of the Niagara particle associated with this Mass entity.
 *
 * Used to map a Niagara particle instance to its corresponding Mass entity.
 */
USTRUCT()
struct MAAASSPARTICLE_API FMPNiagaraParticleIDFragment : public FMassFragment
{
	GENERATED_BODY()

	/** The Niagara particle ID for this entity. */
	UPROPERTY()
	int32 NiagaraParticleID = -1;
};