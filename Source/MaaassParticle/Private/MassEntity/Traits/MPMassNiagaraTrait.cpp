// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPMassNiagaraTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "MPNiagaraParticleIDFragment.h"
#include "MPNiagaraComponentFragment.h"
#include <MassMovementFragments.h>
#include "MPAnimStateFragment.h"
#include "MPEntityAgeFragment.h"
#include "MPNeedsInitializationTag.h"
#include "MPTriggerVolumeEventFragment.h"
#include "MPTriggerVolumeRequestEventFragment.h"

void UMPMassNiagaraTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	// Map each Niagara particle to a Mass Entity
	BuildContext.AddFragment<FMPNiagaraParticleIDFragment>();
	BuildContext.AddFragment<FMPEntityAgeFragment>();
	// Store reference to the NiagaraComponent driving the particles
	BuildContext.AddFragment<FMPNiagaraComponentFragment>();
	// Include animation state control for particle-driven animation
	BuildContext.AddFragment<FMPAnimStateFragment>();

	BuildContext.AddFragment<FMPTriggerVolumeEventFragment>();
	BuildContext.AddFragment<FMPTriggerVolumeRequestEventFragment>();

	// Tag entities as needing initialization before first simulation step
	BuildContext.AddTag<FMPNeedsInitializationTag>();

	// Standard transform for positioning in the world
	//BuildContext.AddFragment<FTransformFragment>();
	// Velocity for movement systems
	//BuildContext.AddFragment<FMassVelocityFragment>();
}
