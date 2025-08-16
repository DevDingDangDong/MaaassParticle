// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraDataInterface.h"
#include "NiagaraDataInterfaceLODBAT.generated.h"

/**
 * Instance data structure for LOD BAT (Baked Animation Texture) Data Interface
 */
USTRUCT()
struct FLODBATInstanceData
{
    GENERATED_BODY()
};

// Forward declaration
class UMPAnimToTextureDataAsset;

/**
 * Niagara Data Interface for LOD-based Bone Animation Texture (BAT) systems
 * 
 * This data interface provides access to bone animation texture data within Niagara particle systems,
 * allowing particles to sample animation frames, timing information, and sample rates from 
 * pre-baked animation textures. Supports LOD-based animation selection for performance optimization.
 * 
 * Key Features:
 * - Animation frame range queries (start/end frames)
 * - Sample rate access for proper animation timing
 */
UCLASS(EditInlineNew, BlueprintType, Category = "Niagara")
class MAAASSPARTICLE_API UNiagaraDataInterfaceLODBAT : public UNiagaraDataInterface
{
    GENERATED_BODY()

public:
    UNiagaraDataInterfaceLODBAT(FObjectInitializer const& ObjectInitializer);

    /** 
     * Reference to the Animation to Texture data asset containing baked animation data
     * This asset stores pre-computed animation frames, timing, and metadata
     */
    UPROPERTY(EditAnywhere, Category = "LOD", meta = (DisplayName = "MP Animation To Texture Data Asset"))
    UMPAnimToTextureDataAsset* MPAnimToTextureDataAsset;
    
    //=============================================================================
    // UNiagaraDataInterface Interface Implementation
    //=============================================================================
    
    virtual void PostInitProperties() override;
    virtual void PostLoad() override { Super::PostLoad(); }
    
#if WITH_EDITORONLY_DATA
    virtual void GetFunctionsInternal(TArray<FNiagaraFunctionSignature>& OutFunctions) const override;
#endif
    virtual void GetVMExternalFunction(const FVMExternalFunctionBindingInfo& BindingInfo, void* InstanceData, FVMExternalFunction& OutFunc) override;
    
    virtual bool CanExecuteOnTarget(ENiagaraSimTarget Target) const override { return Target == ENiagaraSimTarget::CPUSim; }
    
    virtual bool InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
    virtual void DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
    virtual int32 PerInstanceDataSize() const override { return sizeof(FLODBATInstanceData); }
    virtual int32 PerInstanceDataPassedToRenderThreadSize() const override { return 0; }

protected:
    //=============================================================================
    // Function Name Constants
    //=============================================================================
    
    inline static const FName GetAnimStartFrameFunctionName = TEXT("GetAnimStartFrame");
    inline static const FName GetAnimEndFrameFunctionName = TEXT("GetAnimEndFrame");
    inline static const FName IsLoopAnimFunctionName = TEXT("IsLoopAnim");
    inline static const FName GetSampleRateFunctionName = TEXT("GetSampleRate");

    //=============================================================================
    // VM Function Declarations
    //=============================================================================
    
    void VMGetAnimStartFrame(FVectorVMExternalFunctionContext& Context);
    void VMGetAnimEndFrame(FVectorVMExternalFunctionContext& Context);
    void VMIsLoopAnim(FVectorVMExternalFunctionContext& Context);
    void VMGetSampleRate(FVectorVMExternalFunctionContext& Context);
    
    //=============================================================================
    // BAT Data Access Functions
    //=============================================================================
    
    /**
     * Gets the starting frame index for a specific animation
     * @param AnimIndex - Index of the animation in the data asset
     * @return Starting frame index, or -1 if invalid
     */
    int32 GetAnimStartFrame(int32 AnimIndex) const;
    
    /**
     * Gets the ending frame index for a specific animation
     * @param AnimIndex - Index of the animation in the data asset
     * @return Ending frame index, or -1 if invalid
     */
    int32 GetAnimEndFrame(int32 AnimIndex) const;

    /**
    * Get Animation is loop
    * @param AnimIndex - Index of the animation in the data asset
    * @return Is Animation Loop
    */
    bool IsLoopAnim(int32 AnimIndex) const;
    
    /**
     * Gets the sample rate of the baked animation data
     * @return Sample rate in frames per second, or -1 if data asset is invalid
     */
    float GetSampleRate() const;
};