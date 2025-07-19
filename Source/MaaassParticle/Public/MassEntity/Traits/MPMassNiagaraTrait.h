// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MPMassNiagaraTrait.generated.h"

/**
 * Defines an entity trait that adds the necessary fragments and tags
 * for Niagara-driven particle-to-entity integration.
 *
 * This trait configures entities with:
 * - Niagara particle ID fragment for mapping particles to entities
 * - Niagara component fragment for keeping a reference to the Niagara system
 * - Animation state fragment for controlling particle animations
 * - Initialization tag to mark entities needing setup
 * - Transform and velocity fragments for movement and positioning
 */
UCLASS()
class MAAASSPARTICLE_API UMPMassNiagaraTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
	
	/**
	 * Builds the entity template by adding required fragments and tags.
	 *
	 * @param BuildContext Context used to assemble the entity template.
	 * @param World        The world instance where these entities will be spawned.
	 */
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
