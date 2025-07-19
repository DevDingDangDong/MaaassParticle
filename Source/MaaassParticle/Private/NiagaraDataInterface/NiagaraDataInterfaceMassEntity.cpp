// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "NiagaraDataInterfaceMassEntity.h"

#include "NiagaraSystemInstance.h"

#define LOCTEXT_NAMESPACE "MassNiagaraDataInterface"

UNiagaraDataInterfaceMassEntity::UNiagaraDataInterfaceMassEntity(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Display, TEXT("Mass Niagara DI Construct"))
}

void UNiagaraDataInterfaceMassEntity::PostInitProperties()
{
	Super::PostInitProperties();

	// Register this data interface type with Niagara's type registry
	// This allows the interface to be used as parameters and variables in Niagara graphs
	if (HasAnyFlags(RF_ClassDefaultObject)) 
	{
		ENiagaraTypeRegistryFlags Flags = ENiagaraTypeRegistryFlags::AllowAnyVariable | ENiagaraTypeRegistryFlags::AllowParameter;
		FNiagaraTypeRegistry::Register(FNiagaraTypeDefinition(GetClass()), Flags);
	}
}

#if WITH_EDITORONLY_DATA
void UNiagaraDataInterfaceMassEntity::GetFunctionsInternal(TArray<FNiagaraFunctionSignature>& OutFunctions) const
{
	// Define function signatures for all available functions that can be called from Niagara graphs
	// These signatures are used during compilation and determine the interface in the Niagara editor
	
    // IsValidIndex - Checks if a particle ID exists in the system
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = IsValidIndexFunctionName;
        Sig.bMemberFunction = true;  // Called on data interface instance
        Sig.bRequiresContext = false;  // No execution context needed
    	
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
        Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("IsValid")));
        OutFunctions.Add(Sig);
    }

    // AddParticleData - Creates new particle with position and velocity data
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = AddParticleDataFunctionName;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
    	
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("Position")));
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("Velocity")));
    	Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("AddSuccess")));
        OutFunctions.Add(Sig);
    }

    // SetParticleData - Updates existing particle with new position and velocity
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = SetParticleDataFunctionName;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
    	
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("Position")));
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("Velocity")));
    	Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("SetSuccess")));
        OutFunctions.Add(Sig);
    }

	// SetParticleAnimState - Updates existing particle with new AnimState
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = SetParticleAnimStateDataFunctionName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;

		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("AnimState")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("SetSuccess")));
		OutFunctions.Add(Sig);
	}

	// RemoveParticleData - Removes particle from the system based on execution condition
    {
    	FNiagaraFunctionSignature Sig;
    	Sig.Name = RemoveParticleDataFunctionName;
    	Sig.bMemberFunction = true;
    	Sig.bRequiresContext = false;
    	
    	Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
    	Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("Execute")));
    	Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
    	Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("RemoveSuccess")));
    	OutFunctions.Add(Sig);
    }

	// GetParticleAnimState - Retrieves current animation state index
	{
    	FNiagaraFunctionSignature Sig;
    	Sig.Name = GetParticleAnimStateFunctionName;
    	Sig.bMemberFunction = true;
    	Sig.bRequiresContext = false;
    	
    	Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
    	Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
    	Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("AnimState")));
    	OutFunctions.Add(Sig);
	}

	// GetParticleSplineIndex - Retrieves spline index that particle is following
	{
    	FNiagaraFunctionSignature Sig;
    	Sig.Name = GetParticleSplineIndexFunctionName;
    	Sig.bMemberFunction = true;
    	Sig.bRequiresContext = false;
    	
    	Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
    	Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
    	Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("SplineIndex")));
    	OutFunctions.Add(Sig);
	}

	// GetParticleSplineProgress - Retrieves progress along spline (0.0 to 1.0)
    {
	    FNiagaraFunctionSignature Sig;
    	Sig.Name = GetParticleSplineProgressName;
    	Sig.bMemberFunction = true;
    	Sig.bRequiresContext = false;

    	Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
    	Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
    	Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetFloatDef(), TEXT("Progress")));

    	OutFunctions.Add(Sig);
    }

	// GetParticlePosition - Retrieves current world position as Vector3
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = GetParticlePositionName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;

		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("Position")));

		OutFunctions.Add(Sig);
	}

	// GetParticleVelocity - Retrieves current velocity vector
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = GetParticleVelocityName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;

		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetVec3Def(), TEXT("Velocity")));

		OutFunctions.Add(Sig);
	}

	// GetParticleOrientation - Retrieves current orientation as quaternion
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = GetParticleOrientationName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;

		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetQuatDef(), TEXT("Quat")));

		OutFunctions.Add(Sig);
	}
	
    // AddPendingParticle - Marks particle for Mass Entity processing
    {
        FNiagaraFunctionSignature Sig;
        Sig.Name = AddPendingParticleFunctionName;
        Sig.bMemberFunction = true;
        Sig.bRequiresContext = false;
    	
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
        Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
    	Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("AddSuccess")));
        OutFunctions.Add(Sig);
    }
	
	// ReportDeadParticle - Reports particle death for cleanup based on execution condition
	{
		FNiagaraFunctionSignature Sig;
		Sig.Name = ReportDeadParticleFunctionName;
		Sig.bMemberFunction = true;
		Sig.bRequiresContext = false;

		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("MassDataInterface")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("ParticleID")));
		Sig.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("Execute")));
		Sig.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("AddSuccess")));
		OutFunctions.Add(Sig);
	}
}
#endif

void UNiagaraDataInterfaceMassEntity::GetVMExternalFunction(const FVMExternalFunctionBindingInfo& BindingInfo, void* InstanceData, FVMExternalFunction& OutFunc)
{
	// Bind Niagara function names to their corresponding VM implementation methods
	// This mapping allows Niagara graphs to call C++ functions during simulation
	
	// Call parent implementation first (important for proper inheritance)
	Super::GetVMExternalFunction(BindingInfo, InstanceData, OutFunc);
	
	// Validate instance data before proceeding with function binding
	if (!InstanceData)
	{
		UE_LOG(LogTemp, Error, TEXT("InstanceData is null in GetVMExternalFunction"));
		return;
	}
	
	// Debug logging for function binding (commented out for performance)
	// UE_LOG(LogTemp, Warning, TEXT("GET EXTERNAL FUNCTION: %s"), *BindingInfo.Name.ToString());
	
	// Map function names to their lambda implementations
	// Each lambda captures 'this' and calls the appropriate VM function
	if (BindingInfo.Name == IsValidIndexFunctionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context) {
			VMIsValidIndex(Context);
		});
	}
	else if (BindingInfo.Name == AddParticleDataFunctionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context) {
			VMAddParticleData(Context);
		});
	}
	else if (BindingInfo.Name == SetParticleDataFunctionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context) {
			VMSetParticleData(Context);
		});
	}
	else if (BindingInfo.Name == SetParticleAnimStateDataFunctionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context) {
			VMSetParticleAnimState(Context);
			});
	}
	else if (BindingInfo.Name == RemoveParticleDataFunctionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context)
		{
			VMRemoveParticleData(Context);
		});
	}
	else if (BindingInfo.Name == GetParticleAnimStateFunctionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context)
		{
			VMGetParticleAnimState(Context);
		});
	}
	else if (BindingInfo.Name == GetParticleSplineIndexFunctionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context)
		{
			VMGetParticleSplineIndex(Context);
		});
	}
	else if (BindingInfo.Name == GetParticleSplineProgressName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context)
		{
			VMGetParticleSplineProgress(Context);
		});
	}
	else if (BindingInfo.Name == GetParticlePositionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context)
		{
			VMGetParticlePosition(Context);
		});
	}
	else if (BindingInfo.Name == GetParticleVelocityName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context)
		{
			VMGetParticleVelocity(Context);
		});
	}
	else if (BindingInfo.Name == GetParticleOrientationName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context)
			{
				VMGetParticleOrientation(Context);
			});
	}
	else if (BindingInfo.Name == AddPendingParticleFunctionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context) {
			VMAddPendingParticle(Context);
		});
	}
	else if (BindingInfo.Name == ReportDeadParticleFunctionName)
	{
		OutFunc = FVMExternalFunction::CreateLambda([this](FVectorVMExternalFunctionContext& Context) {
			VMReportDeadParticle(Context);
			});
	}
}

void UNiagaraDataInterfaceMassEntity::VMIsValidIndex(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for checking if a particle ID exists in the system
	// Processes batches of particles for vectorized execution
	
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
    VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
    VectorVM::FExternalFuncRegisterHandler<FNiagaraBool> OutIsValid(Context);
	
	// Process each particle in the batch
    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        int32 ParticleID = ParticleIDParam.GetAndAdvance();
        // Debug logging for particle validation (commented for performance)
    	// UE_LOG(LogTemp, Display, TEXT("%d"), ParticleID)
        bool bIsValid = IsValidIndex(ParticleID, InstData.Get());
        *OutIsValid.GetDestAndAdvance() = bIsValid;
    }
}

void UNiagaraDataInterfaceMassEntity::VMAddParticleData(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for adding new particle data to the system
	// Creates new particle with position and velocity, initializes default values
	
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
    VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	
	// Extract position components separately (Niagara passes Vector3 as individual floats)
	VectorVM::FExternalFuncInputHandler<float> PositionXParam(Context);
	VectorVM::FExternalFuncInputHandler<float> PositionYParam(Context);
	VectorVM::FExternalFuncInputHandler<float> PositionZParam(Context);
	VectorVM::FExternalFuncInputHandler<float> VelocityXParam(Context);
	VectorVM::FExternalFuncInputHandler<float> VelocityYParam(Context);
	VectorVM::FExternalFuncInputHandler<float> VelocityZParam(Context);
    
	VectorVM::FExternalFuncRegisterHandler<FNiagaraBool> OutAddSuccess(Context);

	// Process each particle instance in the batch
	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		int32 ParticleID = ParticleIDParam.GetAndAdvance();
        
		// Reconstruct vectors from individual float components
		float PosX = PositionXParam.GetAndAdvance();
		float PosY = PositionYParam.GetAndAdvance();
		float PosZ = PositionZParam.GetAndAdvance();
		float VelX = VelocityXParam.GetAndAdvance();
		float VelY = VelocityYParam.GetAndAdvance();
		float VelZ = VelocityZParam.GetAndAdvance();
    	
        // Create new particle data structure and initialize
        FNDI_MassEntityParticleData* ParticleData = new FNDI_MassEntityParticleData();
		
    	ParticleData->ParticleID = ParticleID;
		ParticleData->Position = FVector3f(PosX, PosY, PosZ);
		ParticleData->Velocity = FVector3f(VelX, VelY, VelZ);
		ParticleData->SplineIndex = 0;  // Default spline index

		// Add to system and report success/failure
        bool bAddSuccess = AddParticleData(InstData.Get(), ParticleID, ParticleData);
    	*OutAddSuccess.GetDestAndAdvance() = bAddSuccess;
    }
}

void UNiagaraDataInterfaceMassEntity::VMSetParticleData(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for updating existing particle data
	// Modifies position and velocity of existing particles
	
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
    VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncInputHandler<float> PositionXParam(Context);
	VectorVM::FExternalFuncInputHandler<float> PositionYParam(Context);
	VectorVM::FExternalFuncInputHandler<float> PositionZParam(Context);
	VectorVM::FExternalFuncInputHandler<float> VelocityXParam(Context);
	VectorVM::FExternalFuncInputHandler<float> VelocityYParam(Context);
	VectorVM::FExternalFuncInputHandler<float> VelocityZParam(Context);
	
	VectorVM::FExternalFuncRegisterHandler<FNiagaraBool> OutSetSuccess(Context);

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        int32 ParticleID = ParticleIDParam.GetAndAdvance();
    	
    	// Extract new position and velocity values
    	float PosX = PositionXParam.GetAndAdvance();
    	float PosY = PositionYParam.GetAndAdvance();
    	float PosZ = PositionZParam.GetAndAdvance();
    	float VelX = VelocityXParam.GetAndAdvance();
    	float VelY = VelocityYParam.GetAndAdvance();
    	float VelZ = VelocityZParam.GetAndAdvance();

    	// TODO: Consider extracting this update logic into a separate function
        FNDI_MassEntityParticleData* ParticleData = GetParticleData(InstData.Get(), ParticleID);

    	bool bSetSuccess = false;
    	if (ParticleData)
    	{
    		// Update particle position and velocity
    		ParticleData->Position = FVector3f(PosX, PosY, PosZ);
    		ParticleData->Velocity = FVector3f(VelX, VelY, VelZ);

	        bSetSuccess = SetParticleData(InstData.Get(), ParticleID, ParticleData);
    	}
    	
    	*OutSetSuccess.GetDestAndAdvance() = bSetSuccess;
    }
}

void UNiagaraDataInterfaceMassEntity::VMRemoveParticleData(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for conditional particle removal
	// Only removes particles when Execute parameter is true
	
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
    VectorVM::FExternalFuncInputHandler<bool> ExecuteParam(Context);
    VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncRegisterHandler<FNiagaraBool> OutAddSuccess(Context);

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
    	bool bIsExecute = ExecuteParam.GetAndAdvance();
    	bool bAddSuccess;
        int32 ParticleID = ParticleIDParam.GetAndAdvance();
        
    	// Only perform removal if execution condition is met
    	if (bIsExecute)
    	{
    		bAddSuccess = RemoveParticleData(ParticleID, InstData.Get());
    	}
    	else
    	{
    		bAddSuccess = false;
    	}
    	*OutAddSuccess.GetDestAndAdvance() = bAddSuccess;
    }
}

void UNiagaraDataInterfaceMassEntity::VMSetParticleAnimState(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for updating existing particle data
	// Modifies AnimState

	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
	VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncInputHandler<int32> AnimStateParam(Context);

	VectorVM::FExternalFuncRegisterHandler<FNiagaraBool> OutSetSuccess(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		int32 ParticleID = ParticleIDParam.GetAndAdvance();
		int32 AnimState = AnimStateParam.GetAndAdvance();

		bool bSetSuccess = false;

		bSetSuccess = SetParticleAnimationState(InstData, ParticleID, AnimState);

		*OutSetSuccess.GetDestAndAdvance() = bSetSuccess;
	}
}

void UNiagaraDataInterfaceMassEntity::VMGetParticleAnimState(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for retrieving particle animation state
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
	VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncRegisterHandler<int32> OutAnimState(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		int32 ParticleID = ParticleIDParam.GetAndAdvance();
		int32 AnimState = GetParticleAnimState(InstData.Get(), ParticleID);
		*OutAnimState.GetDestAndAdvance() = AnimState;
	}
}

void UNiagaraDataInterfaceMassEntity::VMGetParticleSplineIndex(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for retrieving particle spline index
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
	VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncRegisterHandler<int32> OutSplineIndex(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		int32 ParticleID = ParticleIDParam.GetAndAdvance();
		int32 SplineIndex = GetParticleSplineIndex(InstData.Get(), ParticleID);
		*OutSplineIndex.GetDestAndAdvance() = SplineIndex;
	}
}

void UNiagaraDataInterfaceMassEntity::VMGetParticleSplineProgress(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for retrieving particle progress along spline (0.0 to 1.0)
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
	VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutAccumulateTime(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		int32 ParticleID = ParticleIDParam.GetAndAdvance();
		float Progress = GetParticleSplineProgress(InstData.Get(), ParticleID);
		*OutAccumulateTime.GetDestAndAdvance() = Progress;
	}
}

void UNiagaraDataInterfaceMassEntity::VMGetParticlePosition(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for retrieving particle world position
	// Returns position as separate X, Y, Z components for Niagara Vector3 compatibility
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
	VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosX(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosY(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosZ(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		int32 ParticleID = ParticleIDParam.GetAndAdvance();
		FVector3f Position = GetParticlePosition(InstData.Get(), ParticleID);
		// Split vector into individual components for Niagara
		*OutPosX.GetDestAndAdvance() = Position.X;
		*OutPosY.GetDestAndAdvance() = Position.Y;
		*OutPosZ.GetDestAndAdvance() = Position.Z;
	}
}

void UNiagaraDataInterfaceMassEntity::VMGetParticleVelocity(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for retrieving particle velocity vector
	// Returns velocity as separate X, Y, Z components
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
	VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosX(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosY(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosZ(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		int32 ParticleID = ParticleIDParam.GetAndAdvance();
		FVector3f Velocity = GetParticleVelocity(InstData.Get(), ParticleID);
		*OutPosX.GetDestAndAdvance() = Velocity.X;
		*OutPosY.GetDestAndAdvance() = Velocity.Y;
		*OutPosZ.GetDestAndAdvance() = Velocity.Z;
	}
}

void UNiagaraDataInterfaceMassEntity::VMGetParticleOrientation(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for retrieving particle orientation quaternion
	// Returns quaternion as separate X, Y, Z, W components
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
	VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosX(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosY(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosZ(Context);
	VectorVM::FExternalFuncRegisterHandler<float> OutPosW(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		int32 ParticleID = ParticleIDParam.GetAndAdvance();
		FQuat Quat = GetParticleOrientation(InstData.Get(), ParticleID);
		// Split quaternion into individual components
		*OutPosX.GetDestAndAdvance() = Quat.X;
		*OutPosY.GetDestAndAdvance() = Quat.Y;
		*OutPosZ.GetDestAndAdvance() = Quat.Z;
		*OutPosW.GetDestAndAdvance() = Quat.W;
	}
}

void UNiagaraDataInterfaceMassEntity::VMAddPendingParticle(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for adding particles to pending list for Mass Entity processing
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
    VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncRegisterHandler<FNiagaraBool> OutAddSuccess(Context);

    for (int32 i = 0; i < Context.GetNumInstances(); ++i)
    {
        int32 ParticleID = ParticleIDParam.GetAndAdvance();
    	bool bAddSuccess = AddPendingParticle(InstData.Get(), ParticleID);
    	*OutAddSuccess.GetDestAndAdvance() = bAddSuccess;
    }
}

void UNiagaraDataInterfaceMassEntity::VMReportDeadParticle(FVectorVMExternalFunctionContext& Context)
{
	// VM implementation for reporting particle death based on execution condition
	VectorVM::FUserPtrHandler<FNDI_MassEntityInstanceData> InstData(Context);
	VectorVM::FExternalFuncInputHandler<int32> ParticleIDParam(Context);
	VectorVM::FExternalFuncInputHandler<bool> ParticleExecuteParam(Context);
	VectorVM::FExternalFuncRegisterHandler<FNiagaraBool> OutReportSuccess(Context);

	for (int32 i = 0; i < Context.GetNumInstances(); ++i)
	{
		const bool ExecuteDead = ParticleExecuteParam.GetAndAdvance();
		int32 ParticleID = ParticleIDParam.GetAndAdvance();

		bool bSuccess = false;

		// Only report death if execution condition is met
		if (ExecuteDead)
		{
			bSuccess = AddDeadParticle(InstData.Get(), ParticleID);
		}

		*OutReportSuccess.GetDestAndAdvance() = bSuccess;
	}
}

bool UNiagaraDataInterfaceMassEntity::InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	// Initialize per-instance data when Niagara system starts up
	// Uses placement new to construct the instance data in pre-allocated memory
	FNDI_MassEntityInstanceData* InstData = new(PerInstanceData) FNDI_MassEntityInstanceData();

	// Initialize collections to empty state
	InstData->PendingParticles.Empty();
	InstData->MassParticlesData.Empty();
	
	UE_LOG(LogTemp, Display, TEXT("Mass Init Per Instance Data"))
	
	return Super::InitPerInstanceData(PerInstanceData, SystemInstance);
}

void UNiagaraDataInterfaceMassEntity::DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	// Clean up per-instance data when Niagara system is destroyed
	if (FNDI_MassEntityInstanceData* InstData = static_cast<FNDI_MassEntityInstanceData*>(PerInstanceData))
	{
		// Explicit cleanup of collections to prevent memory leaks
		InstData->MassParticlesData.Empty();
		InstData->PendingParticles.Empty();
        
		// Call destructor explicitly since we used placement new
		InstData->~FNDI_MassEntityInstanceData();
	}
}

bool UNiagaraDataInterfaceMassEntity::IsValidIndex(const int32 ParticleID, const FNDI_MassEntityInstanceData* InstanceData) const
{
	// Validate particle ID existence in the system
	if (!InstanceData || InstanceData->MassParticlesData.IsEmpty())
	{
		return false;
	}

	return InstanceData->MassParticlesData.Contains(ParticleID);
}

FNDI_MassEntityParticleData* UNiagaraDataInterfaceMassEntity::GetParticleData(FNDI_MassEntityInstanceData* InstanceData, const int32 ParticleID) const
{
	// Retrieve particle data pointer for a given particle ID
	if (!InstanceData || !IsValidIndex(ParticleID, InstanceData))
	{
		return nullptr;
	}

	return InstanceData->MassParticlesData[ParticleID];
}

bool UNiagaraDataInterfaceMassEntity::AddParticleData(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID, FNDI_MassEntityParticleData* InParticleData) const
{
	// Add new particle data to the system
	// Only adds if particle doesn't already exist (prevents duplicates)
	// UE_LOG(LogTemp, Warning, TEXT("Add Particle Data"))
	if (!InstanceData || !InParticleData || IsValidIndex(ParticleID, InstanceData))
	{
		return false;
	}

	InstanceData->MassParticlesData.Add(ParticleID, InParticleData);
	return true;
}

bool UNiagaraDataInterfaceMassEntity::SetParticleData(FNDI_MassEntityInstanceData* InstanceData, const int32 ParticleID, FNDI_MassEntityParticleData* InParticleData) const
{
	// Update existing particle data
	// Only updates if particle already exists
	//UE_LOG(LogTemp, Warning, TEXT("SETParticle?"));
	if (!InstanceData || !InParticleData || !IsValidIndex(ParticleID, InstanceData))
	{
		return false;
	}

	InstanceData->MassParticlesData[ParticleID] = InParticleData;
	return true;
}

bool UNiagaraDataInterfaceMassEntity::SetParticleAnimationState(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID, int32 InAnimState) const
{
	if (!InstanceData || !IsValidIndex(ParticleID, InstanceData))
	{
		return false;
	}

	InstanceData->MassParticlesData[ParticleID]->AnimState = InAnimState;
	return true;
}

bool UNiagaraDataInterfaceMassEntity::RemoveParticleData(int32 ParticleID, FNDI_MassEntityInstanceData* InstanceData) const
{
	// Remove particle data from the system
	if (!InstanceData || !IsValidIndex(ParticleID, InstanceData))
	{
		return false;
	}
	
	InstanceData->MassParticlesData.Remove(ParticleID);
	
	return true;
}

int32 UNiagaraDataInterfaceMassEntity::GetParticleAnimState(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const
{
	// Retrieve animation state for a specific particle
	if (!InstanceData || !IsValidIndex(ParticleID, InstanceData))
	{
		return -1;  // Invalid state indicator
	}

	return InstanceData->MassParticlesData[ParticleID]->AnimState;
}

int32 UNiagaraDataInterfaceMassEntity::GetParticleSplineIndex(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const
{
	// Retrieve spline index that particle is following
	if (!InstanceData || !IsValidIndex(ParticleID, InstanceData))
	{
		return -1;  // No spline indicator
	}

	// Debug logging for spline index retrieval (commented for performance)
	// UE_LOG(LogTemp, Display, TEXT("Get Particle Spline Index %d  ID %d"), InstanceData->MassParticlesData[ParticleID]->SplineIndex, ParticleID);

	return InstanceData->MassParticlesData[ParticleID]->SplineIndex;
}

float UNiagaraDataInterfaceMassEntity::GetParticleSplineProgress(FNDI_MassEntityInstanceData* InstanceData,	int32 ParticleID) const
{
	// Retrieve progress along spline (0.0 to 1.0)
	if (!InstanceData || !IsValidIndex(ParticleID, InstanceData))
	{
		return -1.0f;  // Invalid progress indicator
	}

	return InstanceData->MassParticlesData[ParticleID]->SplineProgress;
}

FVector3f UNiagaraDataInterfaceMassEntity::GetParticlePosition(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const
{
	// Retrieve current world position of particle
	if (!InstanceData || !IsValidIndex(ParticleID, InstanceData))
	{
		return FVector3f::ZeroVector;
	}

	return InstanceData->MassParticlesData[ParticleID]->Position;
}

FVector3f UNiagaraDataInterfaceMassEntity::GetParticleVelocity(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const
{
	// Retrieve current velocity vector of particle
	if (!InstanceData || !IsValidIndex(ParticleID, InstanceData))
	{
		return FVector3f::ZeroVector;
	}

	return InstanceData->MassParticlesData[ParticleID]->Velocity;
}

FQuat UNiagaraDataInterfaceMassEntity::GetParticleOrientation(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const
{
	// Retrieve current orientation quaternion of particle
	if (!InstanceData || !IsValidIndex(ParticleID, InstanceData))
	{
		return FQuat::Identity;
	}
	return InstanceData->MassParticlesData[ParticleID]->Orientation;
}

bool UNiagaraDataInterfaceMassEntity::AddPendingParticle(FNDI_MassEntityInstanceData* InstanceData, const int32 ParticleID)
{
	// Add particle to pending list for Mass Entity system processing
	// Prevents duplicate entries in pending list
	// UE_LOG(LogTemp, Warning, TEXT("Add Pending Data"))
	if (!InstanceData->PendingParticles.Contains(ParticleID))
	{
		InstanceData->PendingParticles.Add(ParticleID);
		return true;
	}

	return false;
}

const TArray<int32>& UNiagaraDataInterfaceMassEntity::GetDeadParticles(const FNDI_MassEntityInstanceData* InstanceData) const
{
	// Retrieve list of particles marked for deletion
	static const TArray<int32> EmptyArray;
	return InstanceData ? InstanceData->DeadParticles : EmptyArray;
}

void UNiagaraDataInterfaceMassEntity::ClearDeadParticles(FNDI_MassEntityInstanceData* InstanceData) const
{
	// Clear the dead particles list after processing
	if (InstanceData)
	{
		InstanceData->DeadParticles.Empty();
	}
}

bool UNiagaraDataInterfaceMassEntity::AddDeadParticle(FNDI_MassEntityInstanceData* InstanceData, int32 ParticleID) const
{
	// Mark particle for deletion and add to dead particles list
	if (InstanceData)
	{
		// Prevent duplicate entries in dead particles list
		if (!InstanceData->DeadParticles.Contains(ParticleID))
		{
			InstanceData->DeadParticles.Add(ParticleID);
		}

		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE