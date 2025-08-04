// Copyright 2025 DevDingDangDong, All Rights Reserved.


#include "MassEntity/Traits/MassGroundFollowingTrait.h"
#include "MassEntityTemplateRegistry.h"


void UMassGroundFollowingTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	auto& Fragment = BuildContext.AddFragment_GetRef<FMPGroundTraceFragment>();

    Fragment.TraceStartUpOffset = TraceStartUpOffset;
    Fragment.TraceEndDownOffset = TraceEndDownOffset;
    Fragment.GroundHeightOffset = GroundHeightOffset;
}
