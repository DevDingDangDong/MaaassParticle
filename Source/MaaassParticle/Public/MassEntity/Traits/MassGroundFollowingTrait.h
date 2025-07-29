// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassGroundFollowingTrait.generated.h"

/**
 * @class UMassGroundFollowingTrait
 * @brief An entity trait that adds the necessary fragments and tags for ground-following behavior.
 * This allows entities to stick to the ground by performing a line trace downwards.
 * The settings for the trace can be configured directly within this trait in the Entity Config asset.
 */
UCLASS()
class MAAASSPARTICLE_API UMassGroundFollowingTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
	

protected:
    /**
     * @brief Overridden from UMassEntityTraitBase. This is where we add our fragments and tags to the entity template.
     * @param BuildContext The context for building the entity template, used to add fragments and tags.
     * @param World The world in which the entity will exist.
     */
    virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
