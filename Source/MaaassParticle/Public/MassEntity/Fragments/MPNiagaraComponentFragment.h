// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MassCommonFragments.h"
#include "NiagaraComponent.h"
#include "MPNiagaraComponentFragment.generated.h"

/**
 * Holds a weak reference to the UNiagaraComponent driving this entity's particles.
 *
 * Enables access to the Niagara system that spawned or controls the particle mapped to the entity.
 */
USTRUCT()
struct MAAASSPARTICLE_API FMPNiagaraComponentFragment : public FMassFragment
{
	GENERATED_BODY()
	
	/** Weak pointer to the Niagara component instance. */
	UPROPERTY()
	TWeakObjectPtr<UNiagaraComponent> NiagaraComponent;
};
