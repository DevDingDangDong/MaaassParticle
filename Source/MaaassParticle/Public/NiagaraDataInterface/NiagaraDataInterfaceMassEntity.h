// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraDataInterface.h"
#include "VectorVM.h"
#include "NiagaraDataInterfaceMassEntity.generated.h"

/**
 * Particle data structure that stores all relevant information for a Mass Entity particle.
 * This data is synchronized between the Mass Entity system and Niagara particle simulation.
 */
USTRUCT()
struct FNDI_MassEntityParticleData
{
    GENERATED_BODY()
	
    /** Unique identifier for this particle instance */
    int32 ParticleID = -1;

    /** Current animation state index for this particle */
	int32 AnimState = -1;
	
    /** Index of the spline this particle is following (-1 if not following any spline) */
	int32 SplineIndex = -1;
	
    /** Accumulated time along the spline movement */
	float SplineAccumulateTime = 0.0f;
	
    /** Progress along the spline (0.0 to 1.0) */
	float SplineProgress = 0.0f;
	
    /** Current world position of the particle */
    FVector3f Position = FVector3f::ZeroVector;
    
    /** Current velocity vector of the particle */
	FVector3f Velocity = FVector3f::ZeroVector;
	
    /** Current orientation quaternion of the particle */
	FQuat Orientation = FQuat::Identity;
};

/**
 * Per-instance data structure for the Mass Niagara Data Interface.
 * Manages all particle data, pending particles, and dead particles for a single Niagara system instance.
 */
USTRUCT()
struct FNDI_MassEntityInstanceData
{
	GENERATED_BODY()
	
    /** 
     * Map of active particle data indexed by ParticleID.
     * TODO: Consider using TMap with hash for better performance with large particle counts.
     */
	TSortedMap<int32, FNDI_MassEntityParticleData*> MassParticlesData;
	
    /** Array of particle IDs that are waiting to be processed by the Mass Entity system */
	TArray<int32> PendingParticles;
	
    /** Array of particle IDs that have been marked for deletion */
	TArray<int32> DeadParticles;

    ~FNDI_MassEntityInstanceData()
    {
        for (auto& Pair : MassParticlesData)
        {
            delete Pair.Value;
        }
        MassParticlesData.Empty();
    }
};

/**
 * Niagara Data Interface for bridging Mass Entity simulation with Niagara particle systems.
 * 
 * This data interface provides CPU-side functionality to:
 * - Synchronize particle data between Mass Entity and Niagara systems
 * - Manage particle lifecycle (creation, update, deletion)
 * - Handle spline-based movement and animation states
 * - Provide real-time data access for crowd simulation
 * 
 * The interface operates on CPU simulation target only and provides VM functions
 * that can be called from Niagara graphs to interact with Mass Entity data.
 */
UCLASS(EditInlineNew, Category = "MassCrowd", CollapseCategories, meta = (DisplayName = "Mass Niagara"))
class MAAASSPARTICLE_API UNiagaraDataInterfaceMassEntity : public UNiagaraDataInterface
{
    GENERATED_BODY()

public:
    /**
     * Constructor for the Mass Niagara Data Interface.
     * @param ObjectInitializer Standard UE object initializer
     */
	UNiagaraDataInterfaceMassEntity(FObjectInitializer const& ObjectInitializer);
	
	//~ Begin UNiagaraDataInterface Interface
	virtual void PostInitProperties() override;
	virtual void PostLoad() override { Super::PostLoad(); }
	
	/**
     * Defines all available functions that can be called from Niagara graphs.
     * Only compiled in editor builds as function signatures are baked into compiled assets.
     */
#if WITH_EDITORONLY_DATA
	virtual void GetFunctionsInternal(TArray<FNiagaraFunctionSignature>& OutFunctions) const override;
#endif

    /**
     * Binds VM external functions to their implementation methods.
     * Called when the Niagara system needs to execute a function on the CPU.
     */
	virtual void GetVMExternalFunction(const FVMExternalFunctionBindingInfo& BindingInfo, void* InstanceData, FVMExternalFunction& OutFunc) override;

    /** Restricts execution to CPU simulation only */
	virtual bool CanExecuteOnTarget(ENiagaraSimTarget Target) const override { return Target == ENiagaraSimTarget::CPUSim; }

	virtual bool InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
	virtual void DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
	virtual int32 PerInstanceDataSize() const override { return sizeof(FNDI_MassEntityInstanceData); }
	virtual int32 PerInstanceDataPassedToRenderThreadSize() const override { return 0; }
	//~ End UNiagaraDataInterface Interface

	// ========================================================================
	// Particle Data Management Functions
	// ========================================================================
	
    /**
     * Checks if a particle ID exists in the instance data.
     * @param ParticleID The particle ID to validate
     * @param InstanceData The instance data to check against
     * @return True if the particle ID exists and is valid
     */
	bool IsValidIndex(int32 ParticleID, const FNDI_MassEntityInstanceData* InstanceData) const;
	
    /**
     * Gets all particle data for external access.
     * @param InstanceData The instance data containing particle information
     * @return Copy of the particle data map
     */
	TSortedMap<int32, FNDI_MassEntityParticleData*>& GetParticlesData(FNDI_MassEntityInstanceData* InstanceData) { return InstanceData->MassParticlesData; }
	
    /**
     * Retrieves particle data for a specific particle ID.
     * @param ParticleID The ID of the particle to retrieve
     * @param InstanceData The instance data to search in
     * @return Pointer to particle data, nullptr if not found
     */
	FNDI_MassEntityParticleData* GetParticleData(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const;
	
    /**
     * Adds new particle data to the system.
     * @param ParticleID Unique identifier for the new particle
     * @param InParticleData Particle data to add (takes ownership)
     * @param InstanceData Instance data to add to
     * @return True if particle was successfully added
     */
	bool AddParticleData(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID, FNDI_MassEntityParticleData* InParticleData) const;
	
    /**
     * Updates existing particle data.
     * @param ParticleID ID of the particle to update
     * @param InParticleData New particle data to set
     * @param InstanceData Instance data containing the particle
     * @return True if particle was successfully updated
     */
	bool SetParticleData(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID, FNDI_MassEntityParticleData* InParticleData) const;
	
    bool SetParticleAnimationState(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID, int32 InAnimState) const;

    /**
     * Removes particle data from the system.
     * @param ParticleID ID of the particle to remove
     * @param InstanceData Instance data to remove from
     * @return True if particle was successfully removed
     */
	bool RemoveParticleData(int32 ParticleID, FNDI_MassEntityInstanceData* InstanceData) const;

	// ========================================================================
	// Particle Property Getters
	// ========================================================================
	
    /**
     * Gets the current animation state of a particle.
     * @param InstanceData Instance data containing particle information
     * @param ParticleID ID of the particle to query
     * @return Animation state index, -1 if particle not found
     */
	int32 GetParticleAnimState(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const;
	
    /** DEPRECATED(Use ZoneGraph Instead)
     * Gets the spline index that a particle is following.
     * @param InstanceData Instance data containing particle information
     * @param ParticleID ID of the particle to query
     * @return Spline index, -1 if particle not found or not following a spline
     */
	int32 GetParticleSplineIndex(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const;
	
    /** DEPRECATED(Use ZoneGraph Instead)
     * Gets the progress along the spline for a particle.
     * @param InstanceData Instance data containing particle information
     * @param ParticleID ID of the particle to query
     * @return Spline progress (0.0 to 1.0), -1 if particle not found
     */
	float GetParticleSplineProgress(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const;
	
    /**
     * Gets the current world position of a particle.
     * @param InstanceData Instance data containing particle information
     * @param ParticleID ID of the particle to query
     * @return World position, zero vector if particle not found
     */
	FVector3f GetParticlePosition(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const;
	
    /**
     * Gets the current velocity of a particle.
     * @param InstanceData Instance data containing particle information
     * @param ParticleID ID of the particle to query
     * @return Velocity vector, zero vector if particle not found
     */
	FVector3f GetParticleVelocity(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const;
	
    /**
     * Gets the current orientation of a particle.
     * @param InstanceData Instance data containing particle information
     * @param ParticleID ID of the particle to query
     * @return Orientation quaternion, identity if particle not found
     */
	FQuat GetParticleOrientation(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const;

	// ========================================================================
	// Pending Particle Management
	// ========================================================================
	
    /**
     * Gets the list of particles waiting to be processed by Mass Entity system.
     * @param InstanceData Instance data to query
     * @return Array of pending particle IDs (empty array if InstanceData is null)
     */
	TArray<int32>& GetPendingParticles(FNDI_MassEntityInstanceData* InstanceData) const
	{
		static TArray<int32> EmptyArray;
		return InstanceData ? InstanceData->PendingParticles : EmptyArray;
	}
	
    /**
     * Adds a particle to the pending list for Mass Entity processing.
     * @param InstanceData Instance data to modify
     * @param ParticleID ID of the particle to mark as pending
     * @return True if particle was successfully added to pending list
     */
	bool AddPendingParticle(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID);
	
    /**
     * Clears all pending particles from the list.
     * @param InstanceData Instance data to modify
     */
	void ClearPendingParticles(FNDI_MassEntityInstanceData* InstanceData) { InstanceData->PendingParticles.Empty(); }

	// ========================================================================
	// Dead Particle Management
	// ========================================================================
	
    /**
     * Gets the list of particles that have been marked for deletion.
     * @param InstanceData Instance data to query
     * @return Reference to array of dead particle IDs
     */
	const TArray<int32>& GetDeadParticles(const FNDI_MassEntityInstanceData* InstanceData) const;
	
    /**
     * Marks a particle as dead for cleanup.
     * @param InstanceData Instance data to modify
     * @param ParticleID ID of the particle to mark as dead
     * @return True if particle was successfully marked as dead
     */
	bool AddDeadParticle(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const;
	
    /**
     * Clears all dead particles from the list.
     * @param InstanceData Instance data to modify
     */
	void ClearDeadParticles(FNDI_MassEntityInstanceData* InstanceData) const;

protected:
	// ========================================================================
	// Function Name Constants for Niagara Graph Integration
	// ========================================================================
	
	inline static const FName IsValidIndexFunctionName = TEXT("IsValidIndex");
	inline static const FName AddParticleDataFunctionName = TEXT("AddParticleData");
	inline static const FName SetParticleDataFunctionName = TEXT("SetParticleData");
    inline static const FName SetParticleAnimStateDataFunctionName = TEXT("SetParticleAnimState");
	inline static const FName RemoveParticleDataFunctionName = TEXT("RemoveParticleData");
	inline static const FName ReportDeadParticleFunctionName = TEXT("ReportDeadParticle");
	inline static const FName GetParticleAnimStateFunctionName = TEXT("GetParticleAnimState");
	inline static const FName GetParticleSplineIndexFunctionName = TEXT("GetParticleSplineIndex");
	inline static const FName GetParticleSplineProgressName = TEXT("GetParticleSplineProgress");
	inline static const FName GetParticlePositionName = TEXT("GetParticlePosition");
	inline static const FName GetParticleVelocityName = TEXT("GetParticleVelocity");
	inline static const FName GetParticleOrientationName = TEXT("GetParticleOrientation");
	inline static const FName AddPendingParticleFunctionName = TEXT("AddPendingParticle");
	
	// ========================================================================
	// VM Function Implementations (CPU Execution)
	// ========================================================================

	// Mapping Data
	void VMIsValidIndex(FVectorVMExternalFunctionContext& Context);
	void VMAddParticleData(FVectorVMExternalFunctionContext& Context);
	void VMSetParticleData(FVectorVMExternalFunctionContext& Context);
	void VMRemoveParticleData(FVectorVMExternalFunctionContext& Context);
	
	// Particle Data
    void VMSetParticleAnimState(FVectorVMExternalFunctionContext& Context);
	void VMGetParticleAnimState(FVectorVMExternalFunctionContext& Context);
	void VMGetParticleSplineIndex(FVectorVMExternalFunctionContext& Context);
	void VMGetParticleSplineProgress(FVectorVMExternalFunctionContext& Context);
	void VMGetParticlePosition(FVectorVMExternalFunctionContext& Context);
	void VMGetParticleVelocity(FVectorVMExternalFunctionContext& Context);
	void VMGetParticleOrientation(FVectorVMExternalFunctionContext& Context);

	// Pending Data
	void VMAddPendingParticle(FVectorVMExternalFunctionContext& Context);
	void VMReportDeadParticle(FVectorVMExternalFunctionContext& Context);
};