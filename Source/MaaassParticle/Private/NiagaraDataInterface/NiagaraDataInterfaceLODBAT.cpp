// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "NiagaraDataInterfaceLODBAT.h"
#include "AnimToTexture/MPAnimToTextureDataAsset.h"
#include "VerseVM/VVMInstantiationContext.h"

//=============================================================================
// Construction and Initialization
//=============================================================================

UNiagaraDataInterfaceLODBAT::UNiagaraDataInterfaceLODBAT(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
    ECAnimToTextureDataAsset = nullptr;
	// Initialize with default values - data asset will be set via editor or blueprint
}

void UNiagaraDataInterfaceLODBAT::PostInitProperties()
{
	Super::PostInitProperties();

	// Register this data interface type with Niagara's type registry for editor use
	// This allows the interface to be used as variables and parameters in Niagara scripts
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		ENiagaraTypeRegistryFlags Flags = ENiagaraTypeRegistryFlags::AllowAnyVariable | ENiagaraTypeRegistryFlags::AllowParameter;
		FNiagaraTypeRegistry::Register(FNiagaraTypeDefinition(GetClass()), Flags);
	}
}

//=============================================================================
// Function Signature Registration (Editor Only)
//=============================================================================

#if WITH_EDITORONLY_DATA
void UNiagaraDataInterfaceLODBAT::GetFunctionsInternal(TArray<FNiagaraFunctionSignature>& OutFunctions) const
{
    Super::GetFunctionsInternal(OutFunctions);

    // Register GetAnimStartFrame function signature
    // Input: AnimIndex (int32) -> Output: StartFrame (int32)
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = GetAnimStartFrameFunctionName;
        Sig.bMemberFunction = true;      // This is a member function of the data interface
        Sig.bRequiresContext = false;    // No additional execution context needed
        
        // Add the data interface itself as first parameter (standard for member functions)
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("LODBATDataInterface")));
        // Add animation index parameter
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("AnimIndex")));
        // Define return value - the starting frame index
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("StartFrame")));
        OutFunctions.Add(Sig);
    }

    // Register GetAnimEndFrame function signature
    // Input: AnimIndex (int32) -> Output: EndFrame (int32)
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = GetAnimEndFrameFunctionName;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
        
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("LODBATDataInterface")));
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("AnimIndex")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("EndFrame")));
        OutFunctions.Add(Sig);
    }

    // Register GetSampleRate function signature
    // No input parameters -> Output: SampleRate (float)
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = GetSampleRateFunctionName;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;

        // Only requires the data interface reference
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("LODBATDataInterface")));
        // Returns the sample rate as floating point value
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetFloatDef(), TEXT("SampleRate")));
        OutFunctions.Add(Sig);
    }
}
#endif

//=============================================================================
// VM Function Binding
//=============================================================================

void UNiagaraDataInterfaceLODBAT::GetVMExternalFunction(const FVMExternalFunctionBindingInfo& BindingInfo, void* InstanceData, FVMExternalFunction& OutFunc)
{
    // Validate instance data before proceeding with function binding
    // This prevents crashes from null pointer access in VM functions
    if (!InstanceData)
    {
        UE_LOG(LogTemp, Error, TEXT("InstanceData is null in GetVMExternalFunction"));
        return;
    }
    
    // Bind the appropriate VM function based on the requested function name
    if (BindingInfo.Name == GetAnimStartFrameFunctionName)
    {
        // Create lambda wrapper for VMGetAnimStartFrame
        OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context) {
            VMGetAnimStartFrame(Context);
        });
    }
    else if (BindingInfo.Name == GetAnimEndFrameFunctionName)
    {
        // Create lambda wrapper for VMGetAnimEndFrame
        OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context) {
            VMGetAnimEndFrame(Context);
        });
    }
    else if (BindingInfo.Name == GetSampleRateFunctionName)
    {
        // Create lambda wrapper for VMGetSampleRate
        OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context)
        {
            VMGetSampleRate(Context);
        });
    }
}

//=============================================================================
// VM Function Implementations
//=============================================================================

void UNiagaraDataInterfaceLODBAT::VMGetAnimStartFrame(FVectorVMExternalFunctionContext& Context)
{
    // Set up parameter handlers for vectorized execution
    VectorVM::FUserPtrHandler<FLODBATInstanceData> InstData(Context);      // Per-instance data
    VectorVM::FExternalFuncInputHandler<int32> AnimIndexParam(Context);    // Input: animation index
    VectorVM::FExternalFuncRegisterHandler<int32> OutStartFrame(Context);  // Output: start frame
    
    // Process each instance in the batch (vectorized execution)
    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        int32 AnimIndex = AnimIndexParam.GetAndAdvance();  // Get animation index for this instance
        int32 StartFrame = GetAnimStartFrame(AnimIndex);   // Query the start frame
        
        // Write result to output register and advance to next instance
        *OutStartFrame.GetDestAndAdvance() = StartFrame;
    }
}

void UNiagaraDataInterfaceLODBAT::VMGetAnimEndFrame(FVectorVMExternalFunctionContext& Context)
{
    // Set up parameter handlers for vectorized execution
    VectorVM::FUserPtrHandler<FLODBATInstanceData> InstData(Context);    // Per-instance data
    VectorVM::FExternalFuncInputHandler<int32> AnimIndexParam(Context);  // Input: animation index
    VectorVM::FExternalFuncRegisterHandler<int32> OutEndFrame(Context);  // Output: end frame
    
    // Process each instance in the batch (vectorized execution)
    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        int32 AnimIndex = AnimIndexParam.GetAndAdvance();  // Get animation index for this instance
        int32 EndFrame = GetAnimEndFrame(AnimIndex);       // Query the end frame
        
        // Write result to output register and advance to next instance
        *OutEndFrame.GetDestAndAdvance() = EndFrame;
    }
}

void UNiagaraDataInterfaceLODBAT::VMGetSampleRate(FVectorVMExternalFunctionContext& Context)
{
    // Set up parameter handlers for vectorized execution
    VectorVM::FUserPtrHandler<FLODBATInstanceData> InstData(Context);      // Per-instance data
    VectorVM::FExternalFuncRegisterHandler<float> OutSampleRate(Context);  // Output: sample rate

    // Process each instance in the batch
    // Note: Sample rate is the same for all instances since it's a property of the data asset
    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        float SampleRate = GetSampleRate();  // Query the sample rate
        
        // Write result to output register and advance to next instance
        *OutSampleRate.GetDestAndAdvance() = SampleRate;
    }
}

//=============================================================================
// Per-Instance Data Management
//=============================================================================

bool UNiagaraDataInterfaceLODBAT::InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
    // Initialize per-instance data using placement new
    // This ensures proper construction of the FLODBATInstanceData structure
    FLODBATInstanceData* InstanceData = new(PerInstanceData) FLODBATInstanceData();
    
    // Call parent initialization for any base class setup
	return Super::InitPerInstanceData(PerInstanceData, SystemInstance);
}

void UNiagaraDataInterfaceLODBAT::DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
    // Safely cast and destroy the per-instance data
    if (FLODBATInstanceData* InstData = static_cast<FLODBATInstanceData*>(PerInstanceData))
    {
        // Explicitly call destructor to ensure proper cleanup
        // Memory is managed by Niagara's pool system, so we only call destructor
        InstData->~FLODBATInstanceData();
    }
}

//=============================================================================
// BAT (Baked Animation Texture) Data Access Functions
//=============================================================================

int32 UNiagaraDataInterfaceLODBAT::GetAnimStartFrame(int32 AnimIndex) const
{
    // Validate data asset reference
    if (!ECAnimToTextureDataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetAnimStartFrame: ECAnimToTextureDataAsset is null"));
        return -1;
    }
    
    // Validate animation index bounds
    if (!ECAnimToTextureDataAsset->Animations.IsValidIndex(AnimIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("GetAnimStartFrame: Invalid Anim Index %d, Array Size: %d"), 
               AnimIndex, ECAnimToTextureDataAsset->Animations.Num());
        return -1;
    }

    // Return the start frame for the specified animation
    return ECAnimToTextureDataAsset->Animations[AnimIndex].StartFrame;
}

int32 UNiagaraDataInterfaceLODBAT::GetAnimEndFrame(int32 AnimIndex) const
{
    // Validate data asset reference
    if (!ECAnimToTextureDataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetAnimEndFrame: ECAnimToTextureDataAsset is null"));
        return -1;
    }
    
    // Validate animation index bounds
    if (!ECAnimToTextureDataAsset->Animations.IsValidIndex(AnimIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("GetAnimEndFrame: Invalid Anim Index %d, Array Size: %d"), 
               AnimIndex, ECAnimToTextureDataAsset->Animations.Num());
        return -1;
    }

    // Return the end frame for the specified animation
    return ECAnimToTextureDataAsset->Animations[AnimIndex].EndFrame;
}

float UNiagaraDataInterfaceLODBAT::GetSampleRate() const
{
    // Validate data asset reference
    if (!ECAnimToTextureDataAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetSampleRate: ECAnimToTextureDataAsset is null"));
        return -1.0f;
    }

    // Return the sample rate from the data asset
    // This represents frames per second for the baked animation data
    return ECAnimToTextureDataAsset->SampleRate;
}