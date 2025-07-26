// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ZoneGraphTypes.h"
#include "MPHierarchicalBoundsHashGrid3D.h"
#include "MassEntityHandle.h"
#include "MPTriggerVolumeSubsystem.generated.h"

class UMPTriggerVolumeComponent;

/** Stores runtime grid indexing info for a registered volume */
struct FRuntimeVolumeGridData
{
	int32 GridItemIndex = INDEX_NONE;			// Index of the item in the grid
	FBox CachedBounds;							// Cached bounding box used during registration
};

/**
 * Subsystem responsible for managing trigger volume components in the world.
 *
 * Handles registration, runtime spatial queries, and volume lookup logic for AI or MassEntity agents.
 */
UCLASS()
class MAAASSPARTICLE_API UMPTriggerVolumeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()


protected:
	/** Initializes the subsystem and sets up volume grids */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	/** Registers a trigger volume component into the system */
	void RegisterComponent(UMPTriggerVolumeComponent* Component);
	
	/** Unregisters a previously registered trigger volume component */
	void UnregisterComponent(UMPTriggerVolumeComponent* Component);

	/**
	 * Finds the highest-priority trigger volume component that contains the given world location.
	 * @param Location World-space location to query.
	 * @return Pointer to the best matching component, or nullptr if none overlap.
	 */
	UMPTriggerVolumeComponent* FindBestInteractionSourceAtLocation(const FVector&
		Location) const;

	/**
	 * Finds all trigger volume components that overlap a given location.
	 * Supports both runtime and precomputed static volumes.
	 * @param Location World location to check.
	 * @param OutComponents Array to receive overlapping components.
	 */
	void FindOverlappingVolumeCompsAtLocation(const FVector& Location, TArray<UMPTriggerVolumeComponent*>& OutComponents) const;

	/** Runtime index data for each component, used for spatial updates */
	TMap<TWeakObjectPtr<UMPTriggerVolumeComponent>, FRuntimeVolumeGridData> RuntimeComponentGridData;

private:
	/** Set of volume components registered for runtime spatial queries */
	TSet<TWeakObjectPtr<UMPTriggerVolumeComponent>> RuntimeVolumeComps;

	/** Hierarchical spatial hash grid for runtime volumes */
	TMPHierarchicalBoundsHashGrid3D<UMPTriggerVolumeComponent*> RuntimeVolumeGrid;

	/** Set of components marked for static, precomputed volume-lane overlap */
	TSet<TWeakObjectPtr<UMPTriggerVolumeComponent>> StaticVolumeComps;

	/** Precomputed map of ZoneGraph lanes to overlapping static volume components */
	TMap<FZoneGraphLaneHandle, TArray<UMPTriggerVolumeComponent*>> ZgLaneHandleToStaticVolumeComps;

	/** Tracks where each component is placed within the spatial grid */
	TMap<UMPTriggerVolumeComponent*, TMPHierarchicalBoundsHashGrid3D<UMPTriggerVolumeComponent*>::FCellLocationVolume> ComponentLocations;

	/** Hierarchical grid structure for static (non-runtime) volumes */
	TMPHierarchicalBoundsHashGrid3D<UMPTriggerVolumeComponent*> StaticVolumeGrid;

public:
	/**
	 * Returns the current volume an entity is assigned to.
	 * @param Entity The entity to check.
	 * @return Weak reference to the volume component, or null if none.
	 */
	TWeakObjectPtr<UMPTriggerVolumeComponent> GetVolumeForEntity(const FMassEntityHandle Entity) const;

	/**
	 * Updates or clears the volume currently associated with an entity.
	 * @param Entity The entity to modify.
	 * @param VolumeComponent The new volume, or nullptr to clear.
	 */
	void UpdateEntityCurrentVolume(const FMassEntityHandle Entity, UMPTriggerVolumeComponent* VolumeComponent);

private:
	/** Internal map from entity to the trigger volume they are currently inside */
	TMap<FMassEntityHandle, TWeakObjectPtr<UMPTriggerVolumeComponent>> EntityToVolumeMap;	
};
