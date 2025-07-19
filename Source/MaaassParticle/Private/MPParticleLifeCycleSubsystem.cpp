// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPParticleLifeCycleSubsystem.h"
#include "MassEntityManager.h"
#include "MassEntityTemplate.h"
#include "NiagaraDataInterfaceMassEntity.h"
#include "NiagaraComponent.h"
#include "MassObserverManager.h"
#include "MassEntityTypes.h"
#include "MassEntitySubsystem.h"
#include "MassEntity/Fragments/MPNiagaraParticleIDFragment.h"
#include "MassEntity/Fragments/MPNiagaraComponentFragment.h"
#include "MassEntityConfigAsset.h"
#include "Actors/MPSpawner.h"
#include "MassEntity/Fragments/MPDeletionTags.h"
#include "NiagaraSystemInstanceController.h"
#include "MassSpawnerSubsystem.h"
#include "MassSpawnLocationProcessor.h"

void UMPParticleLifeCycleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MassEntitySubsystem = GetWorld()->GetSubsystem<UMassEntitySubsystem>();
}

void UMPParticleLifeCycleSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UMPParticleLifeCycleSubsystem::Tick(float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UMPParticleLifeCycleSubsystem::Tick);
	Super::Tick(DeltaTime);

	if (!MassEntitySubsystem)
	{
		return;
	}

	// List of dead particle IDs from all spawners in the current frame.
	FMassEntityManager& EntityManager = MassEntitySubsystem->GetMutableEntityManager();

	// Temporary list to remove invalid (destroyed) spawners
	TArray<TWeakObjectPtr<AMPSpawner>> SpawnersToRemove;

	// Iterate through all registered spawners.
	for (const TWeakObjectPtr<AMPSpawner>& SpawnerPtr : RegisteredSpawners)
	{

		AMPSpawner* Spawner = SpawnerPtr.Get();

		// If the spawner is invalid, add it to the removal list.
		if (!Spawner)
		{
			SpawnersToRemove.Add(SpawnerPtr);
			continue;
		}

		// Get information from the spawner.
		UNiagaraComponent* NiagaraComp = Spawner->GetNiagaraComponent();
		UMassEntityConfigAsset* EntityConfigAsset = Spawner->EntityConfigAsset;


		if (!NiagaraComp || !EntityConfigAsset)
		{
			continue; // If necessary information is missing, skip this spawner.
		}

		// Data interface reference
		TArray<UNiagaraDataInterface*> DataInterfaces = NiagaraComp->GetOverrideParameters().GetDataInterfaces();
		UNiagaraDataInterfaceMassEntity* MassNDI = nullptr;

		for (UNiagaraDataInterface* DI : DataInterfaces)
		{
			MassNDI = Cast<UNiagaraDataInterfaceMassEntity>(DI);
			if (MassNDI)
			{
				break;
			}
		}
		if (!MassNDI)
		{
			continue;	// If there is no data interface, no communication.
		}


		FNiagaraSystemInstanceControllerConstPtr Controller = NiagaraComp->GetSystemInstanceController();
		FNDI_MassEntityInstanceData* InstanceData = nullptr;
		if (Controller.IsValid() && MassNDI)
		{
			FNiagaraSystemInstance* SystemInstance = Controller->GetSystemInstance_Unsafe();

			SystemInstance->GetId();
			InstanceData = SystemInstance->FindTypedDataInterfaceInstanceData<FNDI_MassEntityInstanceData>(MassNDI);
		}

		// Add to spawn request queue
		const TArray<int32>& PendingIDs = MassNDI->GetPendingParticles(InstanceData);
		
		// Create entities
		if (PendingIDs.IsEmpty() == false)
		{
			SpawnRequestQueue.Enqueue(MakeTuple(SpawnerPtr, PendingIDs));
			MassNDI->ClearPendingParticles(InstanceData);
		}

		// Add to destruction request queue
		const TArray<int32>& DeadParticleIDs = MassNDI->GetDeadParticles(InstanceData);
		if (DeadParticleIDs.IsEmpty() == false)
		{
			if (TMap<int32, FMassEntityHandle>* ParticleMap = SpawnerToParticleEntityMap.Find(NiagaraComp))
			{
				for (const int32 DeadParticleID : DeadParticleIDs)
				{
					if (FMassEntityHandle* EntityToDestroy = ParticleMap->Find(DeadParticleID))
					{
						DestructionQueue.Enqueue(*EntityToDestroy);
						ParticleMap->Remove(DeadParticleID);
					}
				}
			}
			MassNDI->ClearDeadParticles(InstanceData);
		}
		
	}

	// Remove destroyed spawners from the list.
	if (SpawnersToRemove.Num() > 0)
	{
		for (const TWeakObjectPtr<AMPSpawner>& SpawnerToRemove : SpawnersToRemove)
		{
			RegisteredSpawners.Remove(SpawnerToRemove);
		}
	}

	// Process a certain amount of queued work every frame.
	int32 ProcessedCount = 0;
	// Destruction processing
	while (DestructionQueue.IsEmpty() == false && ProcessedCount < MaxEntitiesToProcessPerFrame)
	{
		FMassEntityHandle EntityToDestroy;
		DestructionQueue.Dequeue(EntityToDestroy);

		if (EntityManager.IsEntityValid(EntityToDestroy))
		{
			EntityManager.AddTagToEntity(EntityToDestroy, FPendingDeletionTag::StaticStruct());
		}
		ProcessedCount++;
	}

	// --- Complete change in creation processing logic ---
	while (!SpawnRequestQueue.IsEmpty() && ProcessedCount < MaxEntitiesToProcessPerFrame)
	{
		TTuple<TWeakObjectPtr<AMPSpawner>, TArray<int32>> SpawnRequest;
		SpawnRequestQueue.Dequeue(SpawnRequest);

		TWeakObjectPtr<AMPSpawner> SpawnerPtr = SpawnRequest.Get<0>();
		TArray<int32> ParticleIDs = SpawnRequest.Get<1>();

		AMPSpawner* Spawner = SpawnerPtr.Get();
		if (!Spawner || ParticleIDs.IsEmpty())
		{
			continue;
		}

		const int32 NumToSpawn = ParticleIDs.Num();
		ProcessedCount += NumToSpawn; // Calculate processed amount

		// Check if SpawnDataGenerator exists
		if (Spawner->SpawnDataGenerator.GeneratorInstance)
		{
			// If Generator exists, request location data generation asynchronously
			UMassEntitySpawnDataGeneratorBase* Generator = Spawner->SpawnDataGenerator.GeneratorInstance;

			// Create a callback delegate. Pass the Spawner and ParticleID list together
			FFinishedGeneratingSpawnDataSignature Delegate = FFinishedGeneratingSpawnDataSignature::CreateUObject(this, &UMPParticleLifeCycleSubsystem::OnSpawnDataGenerated, SpawnerPtr, ParticleIDs);

			// Temporarily create a structure similar to AMassSpawner's EntityTypes and pass it
			FMassSpawnedEntityType EntityType;
			EntityType.EntityConfig = Spawner->EntityConfigAsset;
			TArray<FMassSpawnedEntityType> EntityTypes = { EntityType };

			Generator->Generate(*Spawner, EntityTypes, NumToSpawn, Delegate);
		}
		else
		{
			// If Generator does not exist, create immediately using Niagara particle positions as before
			// In this case, it has the same effect as creating the location data directly and calling OnSpawnDataGenerated
			FMassEntitySpawnDataGeneratorResult Result;
			Result.NumEntities = NumToSpawn;
			Result.EntityConfigIndex = 0; // We only have one EntityType
			Result.SpawnData.InitializeAs<FMassTransformsSpawnData>();

			FMassTransformsSpawnData& Transforms = Result.SpawnData.GetMutable<FMassTransformsSpawnData>();
			Transforms.Transforms.Reserve(NumToSpawn);

			// Need to get the actual particle positions from Niagara (this requires functionality from the MassNiagara module)
			// Here, we use the spawner's location as a temporary measure. The actual implementation needs logic to get particle positions.
			for (int32 i = 0; i < NumToSpawn; ++i)
			{
				FTransform& Transform = Transforms.Transforms.AddDefaulted_GetRef();
				// TODO: Need to implement logic to get the actual particle position.
				Transform.SetLocation(Spawner->GetActorLocation() + FVector(FMath::RandRange(-100, 100), FMath::RandRange(-100, 100), 0));
			}

			OnSpawnDataGenerated({ Result }, SpawnerPtr, ParticleIDs);
		}
	}
}

void UMPParticleLifeCycleSubsystem::RegisterNiagaraSpawner(AMPSpawner* InActor)
{
	if (InActor)
    {
		UE_LOG(LogTemp, Log, TEXT("AECSpawnerActor '%s' is registering to UEasyCrowdSubsystem."), *InActor->GetName());
        RegisteredSpawners.Add(InActor);
    }
}

void UMPParticleLifeCycleSubsystem::UnRegisterNiagaraSpawner(AMPSpawner* InActor)
{
	if (InActor)
	{
		UE_LOG(LogTemp, Log, TEXT("AECSpawnerActor '%s' is unregistering to UEasyCrowdSubsystem."), *InActor->GetName());
		RegisteredSpawners.Remove(InActor);

		// To prevent memory leaks, also remove the associated particle map.
		if (UNiagaraComponent* NiagaraComp = InActor->GetNiagaraComponent())
		{
			SpawnerToParticleEntityMap.Remove(NiagaraComp);
		}
	}
}

void UMPParticleLifeCycleSubsystem::OnSpawnDataGenerated(TConstArrayView<FMassEntitySpawnDataGeneratorResult> Results, TWeakObjectPtr<AMPSpawner> SpawnerPtr, TArray<int32> ParticleIDs)
{
	AMPSpawner* Spawner = SpawnerPtr.Get();
	if (!Spawner || Results.IsEmpty())
	{
		return;
	}

	UMassSpawnerSubsystem* SpawnerSystem = GetWorld()->GetSubsystem<UMassSpawnerSubsystem>();
	if (!SpawnerSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("UEasyCrowdSubsystem: UMassSpawnerSubsystem is not valid!"));
		return;
	}

	FMassEntityManager& EntityManager = MassEntitySubsystem->GetMutableEntityManager();
	UMassEntityConfigAsset* EntityConfigAsset = Spawner->EntityConfigAsset;
	UNiagaraComponent* NiagaraComp = Spawner->GetNiagaraComponent();

	if (!EntityConfigAsset || !NiagaraComp)
	{
		return;
	}

	const FMassEntityTemplate& EntityTemplate = EntityConfigAsset->GetOrCreateEntityTemplate(*GetWorld());
	if (!EntityTemplate.IsValid())
	{
		return;
	}

	// Combine all location data from Results into a single array.
	TArray<FTransform> AllTransforms;
	for (const FMassEntitySpawnDataGeneratorResult& Result : Results)
	{
		if (Result.SpawnData.GetScriptStruct() == FMassTransformsSpawnData::StaticStruct())
		{
			const FMassTransformsSpawnData& TransformsData = Result.SpawnData.Get<FMassTransformsSpawnData>();
			AllTransforms.Append(TransformsData.Transforms);
		}
	}

	const int32 NumToSpawn = FMath::Min(ParticleIDs.Num(), AllTransforms.Num());
	if (NumToSpawn <= 0)
	{
		return;
	}

	// --- 1. Prepare SpawnData to be passed to SpawnEntities. ---
	FMassTransformsSpawnData TransformsData;
	// Copy only as much location data as NumToSpawn.
	TransformsData.Transforms.Append(AllTransforms.GetData(), NumToSpawn);

	FInstancedStruct SpawnData;
	SpawnData.InitializeAs(FMassTransformsSpawnData::StaticStruct(), (const uint8*)&TransformsData);

	// --- 2. Call SpawnerSubsystem->SpawnEntities to create entities and set their locations. ---
	TArray<FMassEntityHandle> SpawnedEntities;
	SpawnerSystem->SpawnEntities(
		EntityTemplate.GetTemplateID(),
		NumToSpawn,
		SpawnData,
		UMassSpawnLocationProcessor::StaticClass(), // This processor handles FMassTransformsSpawnData.
		SpawnedEntities
	);

	// --- 3. Fill in additional custom data for the created entities. ---
	// Location setting is already done by SpawnEntities.
	// Here, we only fill in the Niagara-related data required by our system.
	for (int32 i = 0; i < SpawnedEntities.Num(); ++i)
	{
		const FMassEntityHandle& Entity = SpawnedEntities[i];
		const int32 ParticleID = ParticleIDs[i];

		// Fill in particle ID and component fragment data
		FMPNiagaraParticleIDFragment& ParticleIDFragment = EntityManager.GetFragmentDataChecked<FMPNiagaraParticleIDFragment>(Entity);
		ParticleIDFragment.NiagaraParticleID = ParticleID;

		FMPNiagaraComponentFragment& ComponentFragment = EntityManager.GetFragmentDataChecked<FMPNiagaraComponentFragment>(Entity);
		ComponentFragment.NiagaraComponent = NiagaraComp;

		// Add to tracking map
		SpawnerToParticleEntityMap.FindOrAdd(NiagaraComp).Add(ParticleID, Entity);
	}
}


