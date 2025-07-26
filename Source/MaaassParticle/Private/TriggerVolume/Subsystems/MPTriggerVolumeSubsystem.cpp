// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPTriggerVolumeSubsystem.h"
#include "MPTriggerVolumeComponent.h"
#include "ZoneGraphSubsystem.h"
#include "MPHierarchicalBoundsHashGrid3D.h"
#include "MassEntityManager.h"
#include "MassEntitySubsystem.h"
#include "MassExecutionContext.h"

void UMPTriggerVolumeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Initialize spatial grids with 3 resolution levels
	const TArray<double> CellSizes = { 100.0f, 1000.0f, 10000.0f };
	StaticVolumeGrid.Init(3, CellSizes);
	RuntimeVolumeGrid.Init(3, CellSizes);
}

void UMPTriggerVolumeSubsystem::RegisterComponent(UMPTriggerVolumeComponent* Component)
{
	if (!Component || !Component->GetOwner())
	{
		return;
	}

	if (AActor* Owner = Component->GetOwner())
	{
		RuntimeVolumeComps.Add(Component);

		const FBox Bounds = Owner->GetComponentsBoundingBox(true);
		const int32 ItemIndex = RuntimeVolumeGrid.Add(Component, Bounds, 0);

		// Cache item index and bounds for future removal
		FRuntimeVolumeGridData& GridData = RuntimeComponentGridData.FindOrAdd(Component);
		GridData.GridItemIndex = ItemIndex;
		GridData.CachedBounds = Bounds;
	}
}

void UMPTriggerVolumeSubsystem::UnregisterComponent(UMPTriggerVolumeComponent* Component)
{
	if (!Component) return;

	if (const FRuntimeVolumeGridData* GridData = RuntimeComponentGridData.Find(Component))
	{
		RuntimeVolumeComps.Remove(Component);
		RuntimeVolumeGrid.Remove(GridData->GridItemIndex, GridData->CachedBounds);
		RuntimeComponentGridData.Remove(Component);
	}
}

UMPTriggerVolumeComponent* UMPTriggerVolumeSubsystem::FindBestInteractionSourceAtLocation(const FVector& Location) const
{
	const int32 TargetLevel = 0;
	TArray<UMPTriggerVolumeComponent*> CandidateComponents;
	RuntimeVolumeGrid.FindOverlapping(Location, TargetLevel, CandidateComponents);

	UMPTriggerVolumeComponent* BestComponent = nullptr;
	int32 HighestPriority = -1;

	for (UMPTriggerVolumeComponent* Candidate : CandidateComponents)
	{
		if (Candidate && Candidate->GetOwner())
		{
			// Select the highest priority volume
			if (BestComponent == nullptr || Candidate->Priority > HighestPriority)
			{
				HighestPriority = Candidate->Priority;
				BestComponent = Candidate;
			}
		}
	}

	return BestComponent;
}

void UMPTriggerVolumeSubsystem::FindOverlappingVolumeCompsAtLocation(const FVector& Location, TArray<UMPTriggerVolumeComponent*>& OutComponents) const
{
	OutComponents.Reset();
	// 중복 피하기 위해 TSet 사용.
	TSet<UMPTriggerVolumeComponent*> FoundComponents;

	// 런타임 볼륨 쿼리 (공간 그리드 사용)
	constexpr int32 QueryLevel = 0;

	if (QueryLevel < RuntimeVolumeGrid.NumLevels)
	{
		const FIntVector CellCoords = RuntimeVolumeGrid.GetCellCoords(Location, QueryLevel);
		// Broad-Phase : 쿼리 위치가 속한 셀의 좌표를 계산.
		if (const auto* Cell = RuntimeVolumeGrid.FindCell(CellCoords.X, CellCoords.Y, CellCoords.Z, QueryLevel))
		{
			const auto& AllItems = RuntimeVolumeGrid.GetItems();

			for (const int32 ItemIndex : Cell->ItemIndices)
			{
				if (AllItems.IsAllocated(ItemIndex))
				{
					if (UMPTriggerVolumeComponent* Comp = AllItems[ItemIndex].ID)
					{
						if (AActor* Owner = Comp->GetOwner())
						{
							const FBox Bounds = Owner->GetComponentsBoundingBox(false);
							if (Bounds.IsInsideOrOn(Location))
							{
								FoundComponents.Add(Comp);
							}
						}
					}
				}
			}
		}
	}
	OutComponents = FoundComponents.Array();
}

TWeakObjectPtr<UMPTriggerVolumeComponent> UMPTriggerVolumeSubsystem::GetVolumeForEntity(const FMassEntityHandle Entity) const
{
	return EntityToVolumeMap.FindRef(Entity);
}

void UMPTriggerVolumeSubsystem::UpdateEntityCurrentVolume(const FMassEntityHandle Entity, UMPTriggerVolumeComponent* VolumeComponent)
{
	if (VolumeComponent)
	{
		EntityToVolumeMap.Add(Entity, VolumeComponent);
	}
	else
	{
		EntityToVolumeMap.Remove(Entity);
	}
}
