// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MassSpawnerTypes.h"
#include "Niagara/Internal/NiagaraSystemEmitterState.h"
#include "MPSpawner.generated.h"

class USplineComponentArrayObject;
class USplineComponent;
class UNiagaraDataInterfaceMultiSpline;
class UNiagaraComponent;
class UMassEntityConfigAsset;
class UMPAnimToTextureDataAsset;
class UNiagaraSystem;
class UMPSpawnerDataAsset;

#if WITH_EDITORONLY_DATA
class UBillboardComponent;
#endif

/**
 * Handles asset validation, dynamic parameter updates, and integration
 * with the EasyCrowdSubsystem for entity registration/unregistration.
 */
UCLASS()
class MAAASSPARTICLE_API AMPSpawner : public AActor
{
	GENERATED_BODY()

public:
	/**
	 * Sets default values and creates the Niagara and sprite components.
	 */
	AMPSpawner();

	/**
	 * Called during construction or when a property changes in editor.
	 * Validates settings, updates asset references, and reinitializes the Niagara Component.
	 *
	 * @param Transform The transform used for construction.
	 */
	virtual void OnConstruction(const FTransform& Transform) override;


protected:
	/**
	 * Called when play begins. Activates Niagara and registers with subsystem.
	 */
	virtual void BeginPlay() override;

	/**
	 * Called when play ends. Unregisters from the EasyCrowdSubsystem.
	 *
	 * @param EndPlayReason Reason why play ended.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	/**
	 * Called after actor is loaded. Initializes the spawn data generator if set.
	 */
	virtual void PostLoad() override;

#if WITH_EDITOR
	/**
	 * Called when a property is edited in the editor. Triggers validation.
	 *
	 * @param PropertyChangedEvent Details of the changed property.
	 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
public:

	void UpdateNiagaraComponent();

private:
	/**
	 * Validates current settings in editor, logs warnings, and updates the editor icon.
	 */
	void ValidateSettings();

private:
	UPROPERTY()
	TObjectPtr<UMPSpawnerDataAsset> PrevEasyCrowdAsset;

public:

	/** Crowd asset defining AnimToTextureAsset, NiagaraSystem, and MassEntityConfigAsset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle")
	TObjectPtr<UMPSpawnerDataAsset> MPSpawnerDataAsset;

	/** Animation-to-texture data with LOD support. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset")
	TObjectPtr<UMPAnimToTextureDataAsset> AnimToTextureDataAsset;

	///** Niagara system for crowd particle effects */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset")
	TObjectPtr<UNiagaraSystem> CrowdNiagaraSystem;

	/** Mass Entity Config Asset for configuring entity behaviors */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset")
	TObjectPtr<UMassEntityConfigAsset> EntityConfigAsset;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset", meta = (EditCondition = "LoopBehavior == ENiagaraLoopBehavior::Multiple", ClampMin = "1"))
	int32 LoopCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset", meta = (EditCondition = "LoopBehavior != ENiagaraLoopBehavior::Once", ClampMin = "0"))
	float LoopDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset", meta = (EditCondition = "KillParticleOnLifeHasElapsed == true", ClampMin = "0"))
	float ParticleLifeTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset", meta = (ClampMin = "0"))
	float ParticleScaleRatio = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset")
	FVector ParticleScale = FVector(1.0f, 1.0f, 1.0f);

	/** Configuration for the spawn data generator. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset")
	FMassSpawnDataGenerator SpawnDataGenerator;

	/** Number of particles */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset", meta = (ClampMin = "0"))
	int32 SpawnCount;

	/** Default animation state for spawned entities. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MaaassParticle|MPSpawnerDataAsset", meta = (ClampMin = "0"))
	int32 DefaultAnimState;

	/** Flag tracking first construction pass. */
	bool bIsFirstConstruct = true;

	/** The Niagara component driving the particle simulation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NiagaraComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;

#if WITH_EDITORONLY_DATA

	UPROPERTY()
	TObjectPtr<UBillboardComponent> SpriteComponent;
#endif


public:
	/** Returns the NiagaraComponent instance. */
	UNiagaraComponent* GetNiagaraComponent() const { return NiagaraComponent; }
};
