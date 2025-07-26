// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPVolumeTriggerTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "MPTriggerVolumeRequestEventFragment.h"
#include "MPTriggerVolumeEventFragment.h"

/**
 * Adds the required fragments to the entity template for trigger volume processing.
 *
 * This method is called during entity template construction and ensures that any
 * entity with this trait has the necessary data to handle volume-based interactions.
 */
void UMPVolumeTriggerTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FMPTriggerVolumeEventFragment>();
	BuildContext.AddFragment<FMPTriggerVolumeRequestEventFragment>();
}
