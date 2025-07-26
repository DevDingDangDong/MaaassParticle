// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MPVolumeTriggerTrait.generated.h"

/**
 * Trait that adds components required for volume-trigger interaction.
 *
 * This trait is responsible for adding the necessary fragments (e.g., transform, trigger volume tags)
 * to an entity when it's being built from a template. Typically used for agents that should respond
 * to trigger volume enter/exit logic.
 */
UCLASS()
class MAAASSPARTICLE_API UMPVolumeTriggerTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
protected:
	/**
	 * Called when this trait is applied to an entity template.
	 * Adds required fragments and shared data for trigger volume processing.
	 *
	 * @param BuildContext Context used to define what fragments are added to the entity.
	 * @param World Reference to the world where the entity is being constructed.
	 */
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
