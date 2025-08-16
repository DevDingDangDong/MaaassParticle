// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MassEntityHandle.h"
#include "MassEntitySpawnDataGeneratorBase.h"
#include "MPParticleLifeCycleSubsystem.generated.h"




class AMPSpawner;
class UNiagaraComponent;
class UMassEntitySubsystem;

/**
 * Implements the EasyCrowd world subsystem.
 *
 * Manages registration of Niagara spawner actors, processes spawn and destruction
 * requests by interfacing with the MassEntitySubsystem, and executes queued operations each frame.
 */
UCLASS()
class MAAASSPARTICLE_API UMPParticleLifeCycleSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Initializes the subsystem and caches the MassEntitySubsystem pointer.
	 *
	 * @param Collection The collection of subsystems for this world.
	 */
	virtual void Initialize(FSubsystemCollectionBase&
		Collection) override;
	/**
	 * Deinitializes the subsystem.
	 */
	virtual void Deinitialize() override;
	/**
	 * Called every frame to process spawn and destruction queues.
	 *
	 * @param DeltaTime Time elapsed since last tick.
	 */
	virtual void Tick(float DeltaTime) override;
	
	/**
	 * Determines whether this subsystem should be created for the given outer.
	 *
	 * @param Outer The outer object to evaluate.
	 * @return true if the subsystem should be created; otherwise false.
	 */
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	
	/**
	 * Retrieves the profiling stat ID for this subsystem's Tick.
	 *
	 * @return The stat ID used by the CPU profiler.
	 */
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UMPParticleLifeCycleSubsystem, STATGROUP_Tickables);
	}

	
	/**
	 * Registers a Niagara spawner actor with the subsystem.
	 * Function called by AMPSpawnerActor to register itself with the subsystem.
	 * 
	 * @param InActor The AMPTestSpawner actor to register.
	 */
	void RegisterNiagaraSpawner(AMPSpawner* InActor);
	// Function called by AMPSpawnerActor when it is destroyed to unregister from the subsystem.
	void UnRegisterNiagaraSpawner(AMPSpawner* InActor);

	void OnSpawnDataGenerated(TConstArrayView<FMassEntitySpawnDataGeneratorResult> Results, TWeakObjectPtr<AMPSpawner> SpawnerPtr, TArray<int32> ParticleIDs);

protected:
	// List of spawner actors to manage.
	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<AMPSpawner>> RegisteredSpawners;
	
	// Cached pointer to MassEntitySubsystem.
	UPROPERTY(Transient)
	TObjectPtr<UMassEntitySubsystem> MassEntitySubsystem;

private:
	// Map that maps particle IDs to MassEntityHandlers for each NiagaraComponent.
	TMap<TWeakObjectPtr<UNiagaraComponent>, TMap<int32, FMassEntityHandle>> SpawnerToParticleEntityMap;
	
	TQueue<TPair<TWeakObjectPtr<AMPSpawner>, int32>> SpawnQueue;
	TQueue<TTuple<TWeakObjectPtr<AMPSpawner>, TArray<int32>>> SpawnRequestQueue;
	TQueue<FMassEntityHandle> DestructionQueue;

	UPROPERTY(EditDefaultsOnly, Category = "Crowd Contorl")
	int32 MaxEntitiesToProcessPerFrame = 200;
};
