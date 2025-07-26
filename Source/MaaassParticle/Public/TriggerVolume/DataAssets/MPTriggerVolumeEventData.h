// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MPTriggerVolumeTaskBase.h"
#include "MPTriggerVolumeEventData.generated.h"

/**
 * Data asset that defines a list of trigger volume tasks to execute.
 *
 * This is used to define logic that should be run when an entity enters or exits
 * a trigger volume. Tasks can include actions like sending StateTree events,
 * adding tags, or other custom logic.
 */
UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "MP TriggerVolume Event Data"))
class MAAASSPARTICLE_API UMPTriggerVolumeEventData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/**
	 * The list of tasks to execute when this event data is triggered.
	 * Each task is executed in order during the volume interaction.
	 */
	UPROPERTY(EditAnywhere, Instanced, Category = "Tasks", meta = (DisplayName = "TriggerVolume Tasks"))
	TArray<TObjectPtr<UMPTriggerVolumeTaskBase>> Tasks;
};
