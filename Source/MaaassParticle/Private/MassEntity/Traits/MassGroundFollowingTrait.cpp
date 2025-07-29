// Copyright 2025 DevDingDangDong, All Rights Reserved.


#include "MassEntity/Traits/MassGroundFollowingTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "MPGroundTraceFragment.h"

void UMassGroundFollowingTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FMPGroundTraceFragment>();
}
