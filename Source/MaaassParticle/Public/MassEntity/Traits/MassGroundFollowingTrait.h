// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MPGroundTraceFragment.h"
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
	
public:
    /**
     * @brief Settings for the ground-following behavior.
     * These values will be copied to the FMPGroundTraceFragment for each entity.
     */

    UPROPERTY(EditAnywhere, Category = "Ground Following")
    float TraceStartUpOffset = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Ground Following")
    float TraceEndDownOffset = 2000.f;

    UPROPERTY(EditAnywhere, Category = "Ground Following")
    float GroundHeightOffset = 5.f;

protected:
    /**
     * @brief Overridden from UMassEntityTraitBase. This is where we add our fragments and tags to the entity template.
     * @param BuildContext The context for building the entity template, used to add fragments and tags.
     * @param World The world in which the entity will exist.
     */
    virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
