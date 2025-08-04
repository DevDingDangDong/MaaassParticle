// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimSequence.h"
#include "MPAnimSequenceInfo.generated.h"

class UAnimSequence;
/**
 * @struct FMPAnimSequenceInfo
 * @brief A data structure that pairs an Animation Sequence with its corresponding loop setting.
 *
 * This struct is designed to manage a list of animations where each animation requires
 * its own individual configuration, such as whether it should loop.
 */
USTRUCT(BlueprintType)
struct MAAASSPARTICLE_API FMPAnimSequenceInfo
{
	GENERATED_BODY()
public:
	/**
	 * @brief The Animation Sequence asset to be played.
	 *
	 * This will appear in the Details panel as an asset picker slot, allowing users
	 * to select or drag-and-drop an Animation Sequence asset.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimSequence* AnimationSequence;

	/**
	 * @brief Determines whether this animation should loop when played.
	 *
	 * This will be displayed as a checkbox in the Details panel. If true, the animation will repeat.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsLoop;

	FMPAnimSequenceInfo() : AnimationSequence(nullptr), bIsLoop(true) {}
};
