// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "Actors/MPSpawner.h"
#include "MPParticleLifeCycleSubsystem.h"
#include "MPSpawnerDataAsset.h"
#if WITH_EDITOR
#include "Components/BillboardComponent.h"
#endif
#include "NiagaraComponent.h"
#include "UObject/FastReferenceCollector.h"
#include "NiagaraDataInterface/NiagaraDataInterfaceMassEntity.h"
#include "NiagaraSystemInstanceController.h"
#include "AnimToTexture/MPAnimToTextureDataAsset.h"
#include "NiagaraDataInterfaceLODBAT.h"

AMPSpawner::AMPSpawner()
{
	// Create and attach components.

	PrimaryActorTick.bCanEverTick = true;

	PrevEasyCrowdAsset = nullptr;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);

#if WITH_EDITOR
	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{
		SpriteComponent->SetupAttachment(RootComponent);
		SpriteComponent->bIsEditorOnly = true; // Set to not be visible in the game
		SpriteComponent->SetHiddenInGame(true);
	}

#endif
}

void AMPSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (IsEditorOnly())
	{
		UE_LOG(LogTemp, Warning, TEXT("AECSpawnerActor '%s' is Editor Only Actor!"), *GetName());
	}
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("AECSpawnerActor '%s' is Editor Only Actor!"), *GetName());
	}

	ValidateSettings();

	
	// EasyCrowdAsset Setting

	if (MPSpawnerDataAsset != PrevEasyCrowdAsset) 
	{
		if (MPSpawnerDataAsset == nullptr)
		{
			AnimToTextureDataAsset = nullptr;
			CrowdNiagaraSystem = nullptr;

			EntityConfigAsset = nullptr;

			DefaultAnimState = 0;

			LoopBehavior = ENiagaraLoopBehavior::Once;
			LoopCount = 1;
			LoopDuration = 1.0f;

			SpawnDataGenerator = FMassSpawnDataGenerator();

			SpawnCount = 0;

			PrevEasyCrowdAsset = nullptr;
			
		}
		else
		{
			if (UMPAnimToTextureDataAsset* AnimToTextureDataAssetPtr = MPSpawnerDataAsset->AnimToTextureDataAsset.Get())
			{
				AnimToTextureDataAsset = AnimToTextureDataAssetPtr;
			}
			
			if (UNiagaraSystem* CrowdNiagaraSystemPtr = MPSpawnerDataAsset->CrowdNiagaraSystem.Get())
			{
				CrowdNiagaraSystem = CrowdNiagaraSystemPtr;
			}

			if (UMassEntityConfigAsset* EntityConfigAssetPtr = MPSpawnerDataAsset->EntityConfigAsset.Get())
			{
				EntityConfigAsset = EntityConfigAssetPtr;
			}

			DefaultAnimState = MPSpawnerDataAsset->DefaultAnimState;

			LoopBehavior = MPSpawnerDataAsset->LoopBehavior;
			LoopCount = MPSpawnerDataAsset->LoopCount;
			LoopDuration = MPSpawnerDataAsset->LoopDuration;

			if (UMassEntitySpawnDataGeneratorBase* Template = MPSpawnerDataAsset->SpawnDataGenerator.GeneratorInstance)
			{
				UMassEntitySpawnDataGeneratorBase* NewInst = DuplicateObject<UMassEntitySpawnDataGeneratorBase>(Template, this);
				SpawnDataGenerator.GeneratorInstance = NewInst;
			}

			SpawnCount = MPSpawnerDataAsset->SpawnCount;

			PrevEasyCrowdAsset = MPSpawnerDataAsset;
		}
	}

	if (NiagaraComponent)
	{
		NiagaraComponent->SetAutoActivate(false);

		NiagaraComponent->Modify();
		NiagaraComponent->SetAsset(CrowdNiagaraSystem);

		NiagaraComponent->ReinitializeSystem();

		if (CrowdNiagaraSystem)
		{
			NiagaraComponent->SetIntParameter(TEXT("User.SpawnCount"), 1);
			NiagaraComponent->SetIntParameter(TEXT("User.DefaultAnimState"), DefaultAnimState);

			if (UStaticMesh* CustomMesh = AnimToTextureDataAsset->GetStaticMesh())
			{
				NiagaraComponent->SetVariableStaticMesh(TEXT("User.CustomMesh"), CustomMesh);
			}

			UNiagaraDataInterface* NDI = NiagaraComponent->GetDataInterface(TEXT("LODBAT"));
			if (NDI)
			{
				if (UNiagaraDataInterfaceLODBAT* NDILODBAT = Cast<UNiagaraDataInterfaceLODBAT>(NDI))
				{
					if (NDILODBAT->ECAnimToTextureDataAsset != AnimToTextureDataAsset)
					{
						NDILODBAT->ECAnimToTextureDataAsset = AnimToTextureDataAsset;
						NDILODBAT->MarkRenderDataDirty();
					}
				}
			}

			for (const FNiagaraEmitterHandle& Handle : CrowdNiagaraSystem->GetEmitterHandles())
			{
				NiagaraComponent->SetEmitterEnable(Handle.GetName(), false);
			}


			if (LoopBehavior == ENiagaraLoopBehavior::Once && KillParticleOnLifeHasElapsed == false) 
			{
				NiagaraComponent->SetEmitterEnable(EmitterOnceName, true);
			}
			else if(LoopBehavior == ENiagaraLoopBehavior::Once && KillParticleOnLifeHasElapsed == true)
			{
				NiagaraComponent->SetEmitterEnable(EmitterOnceKillAboutLifetimeName, true);
			}
			else if (LoopBehavior == ENiagaraLoopBehavior::Multiple && KillParticleOnLifeHasElapsed == false)
			{
				NiagaraComponent->SetEmitterEnable(EmitterMultipleName, true);
			}
			else if (LoopBehavior == ENiagaraLoopBehavior::Multiple && KillParticleOnLifeHasElapsed == true)
			{
				NiagaraComponent->SetEmitterEnable(EmitterMultipleKillAboutLifetimeName, true);
			}
			else if (LoopBehavior == ENiagaraLoopBehavior::Infinite && KillParticleOnLifeHasElapsed == false)
			{
				NiagaraComponent->SetEmitterEnable(EmitterInfiniteName, true);
			}
			else if (LoopBehavior == ENiagaraLoopBehavior::Infinite && KillParticleOnLifeHasElapsed == true)
			{
				NiagaraComponent->SetEmitterEnable(EmitterInfiniteKillAboutLifetimeName, true);
			}

			NiagaraComponent->SetIntParameter(TEXT("User.LoopCount"), LoopCount);
			NiagaraComponent->SetFloatParameter(TEXT("User.LoopDuration"), LoopDuration);

			NiagaraComponent->SetBoolParameter(TEXT("User.KillParticleOnLifetime"), KillParticleOnLifeHasElapsed);
			NiagaraComponent->SetFloatParameter(TEXT("User.ParticleLifetime"), ParticleLifeTime);
		}
	}

	bIsFirstConstruct = false;
}


// Called when the game starts or when spawned
void AMPSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (NiagaraComponent)
	{
		if (CrowdNiagaraSystem)
		{
			NiagaraComponent->SetIntParameter(TEXT("User.SpawnCount"), SpawnCount);
		}

		NiagaraComponent->Activate(true);
	}
	UWorld* World = GetWorld();
	if (World)
	{
		if (UMPParticleLifeCycleSubsystem* CrowdSubsystem = World->GetSubsystem<UMPParticleLifeCycleSubsystem>())
		{
			CrowdSubsystem->RegisterNiagaraSpawner(this);
		}
	}
	
}

void AMPSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (UMPParticleLifeCycleSubsystem* CrowdSubsystem = World->GetSubsystem<UMPParticleLifeCycleSubsystem>()) {
			CrowdSubsystem->UnRegisterNiagaraSpawner(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void AMPSpawner::PostLoad()
{
	Super::PostLoad();

	if (SpawnDataGenerator.GeneratorClass)
	{
		SpawnDataGenerator.GeneratorInstance = NewObject<UMassEntitySpawnDataGeneratorBase>(this, SpawnDataGenerator.GeneratorClass);
		SpawnDataGenerator.GeneratorClass = nullptr;
		MarkPackageDirty();
	}

}
#if WITH_EDITOR
void AMPSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	ValidateSettings();
}
#endif

void AMPSpawner::ValidateSettings()
{
#if WITH_EDITOR
	// Since we cannot directly access the private SpriteComponent of AMassSpawner,
	// we find it through GetComponents().

	if (!SpriteComponent) return;

	// First, revert to the default icon.
	if (SpriteComponent->GetClass()->GetDefaultObject<UBillboardComponent>()->Sprite)
	{
		SpriteComponent->SetSprite(SpriteComponent->GetClass()->GetDefaultObject<UBillboardComponent>()->Sprite);
	}

	// Start validation
	bool bHasError = false;
	if (EntityConfigAsset == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AECSpawnerActor '%s': EntityConfigAsset is not assigned"), *GetName());
		bHasError = true;
	}

	// TODO : If there is an error, change to the error icon
#endif
}