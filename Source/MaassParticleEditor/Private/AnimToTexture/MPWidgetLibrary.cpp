// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "AnimToTexture/MPWidgetLibrary.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "FileHelpers.h"
#include "Modules/ModuleManager.h"
#include "GenericPlatform/GenericPlatformApplicationMisc.h"
#include "HAL/PlatformApplicationMisc.h"
#include "MaterialEditingLibrary.h"
#include "Materials/MaterialFunctionMaterialLayer.h" 
#include "Materials/MaterialExpressionMaterialAttributeLayers.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialInterface.h"
#include "Actors/MPSpawner.h"
#include "EngineUtils.h"

/**
 * @brief Recursively traverses the material parent chain to collect all unique materials and material instances.
 * @details This helper function is used to ensure that when we duplicate a material instance, we also
 * account for its parent material, and its parent's parent, and so on, up to the root UMaterial.
 * @param InMaterial The starting material or material instance.
 * @param OutAllMaterials The set to populate with all dependent materials.
 */
void CollectMaterialDependencies(UMaterialInterface* InMaterial, TSet<UMaterialInterface*>& OutAllMaterials)
{
	if (!InMaterial || OutAllMaterials.Contains(InMaterial))
	{
		return;
	}

	OutAllMaterials.Add(InMaterial);

	UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(InMaterial);
	if (MaterialInstance)
	{
		CollectMaterialDependencies(MaterialInstance->Parent, OutAllMaterials);
	}
}


UMPAnimToTextureDataAsset* UMPWidgetLibrary::CreateMPAnimToTextureDA(
	const FString& PackagePath,
	const FString& AssetName,
	USkeletalMesh* SkeletalMesh,
	const TArray<FMPAnimSequenceInfo>& SelectedAnims,
	float SampleRate,
	int32 NumDriverTriangles,
	float Sigma,
	bool bEnforcePowerOfTwo,
	EAnimToTexturePrecision Precision,
	EAnimToTextureNumBoneInfluences NumBoneInfluences
)
{
	// Input Validation
	if (PackagePath.IsEmpty() || !FPackageName::IsValidObjectPath(PackagePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid PackagePath: Must not be empty and start with /Game/. Path was: %s"), *PackagePath);
		return nullptr;
	}
	if (AssetName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("AssetName must not be empty."));
		return nullptr;
	}
	if (!IsValid(SkeletalMesh))
	{
		UE_LOG(LogTemp, Error, TEXT("SkeletalMesh is not valid."));
		return nullptr;
	}
	if (SelectedAnims.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectedAnims array is empty."));
		return nullptr;
	}

	// Construct the final asset name with a standard prefix and suffix.
	const FString Prefix = TEXT("DA_");
	const FString Suffix = TEXT("_BAT");

	const FString FinalAssetName = FString::Printf(TEXT("%s%s%s"), *Prefix, *AssetName, *Suffix);

	const FString FullAssetPath = PackagePath + "/" + FinalAssetName;

	UMPAnimToTextureDataAsset* TargetDataAsset = nullptr;

	// Check if an asset with this name already exists.
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(FullAssetPath));

	if (AssetData.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Asset already exists. Loading and updating: %s"), *FullAssetPath);
		TargetDataAsset = Cast<UMPAnimToTextureDataAsset>(StaticLoadObject(UMPAnimToTextureDataAsset::StaticClass(), nullptr, *FullAssetPath));
		if (!TargetDataAsset)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load or cast existing asset: %s"), *FullAssetPath);
			return nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Asset does not exist. Creating new one at: %s"), *FullAssetPath);
		FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
		UObject* NewAssetObject = AssetToolsModule.Get().CreateAsset(FinalAssetName, PackagePath, UMPAnimToTextureDataAsset::StaticClass(), nullptr);

		if (!NewAssetObject)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create Data Asset via AssetTools."));
			return nullptr;
		}
		TargetDataAsset = Cast<UMPAnimToTextureDataAsset>(NewAssetObject);
	}

	if (!TargetDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to obtain a valid Data Asset object."));
		return nullptr;
	}

	// Populate the data asset's properties with the provided parameters.
	TargetDataAsset->SkeletalMesh = SkeletalMesh;
	TargetDataAsset->AnimSequences.Empty();
	TargetDataAsset->bLoopAnims.Empty();
	TargetDataAsset->SampleRate = SampleRate;
	TargetDataAsset->NumDriverTriangles = NumDriverTriangles;
	TargetDataAsset->Sigma = Sigma;
	TargetDataAsset->bEnforcePowerOfTwo = bEnforcePowerOfTwo;
	TargetDataAsset->Precision = Precision;
	TargetDataAsset->NumBoneInfluences = NumBoneInfluences;


	for (const FMPAnimSequenceInfo& Anim : SelectedAnims)
	{
		if (IsValid(Anim.AnimationSequence))
		{
			FAnimToTextureAnimSequenceInfo NewInfo;
			NewInfo.AnimSequence = Anim.AnimationSequence;
			TargetDataAsset->AnimSequences.Add(NewInfo);
			TargetDataAsset->bLoopAnims.Add(Anim.bIsLoop);
		}
	}

	// Mark the asset's package as dirty so it can be saved.
	TargetDataAsset->MarkPackageDirty();

	TArray<UPackage*> PackagesToSave;
	PackagesToSave.Add(TargetDataAsset->GetPackage());

	// Save the package to disk without prompting the user.
	const bool bCheckDirty = false;
	const bool bPromptToSave = false;

	FEditorFileUtils::EPromptReturnCode ReturnCode = FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, bCheckDirty, bPromptToSave);

	if (ReturnCode == FEditorFileUtils::EPromptReturnCode::PR_Success)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully created and saved Data Asset: %s"), *FullAssetPath);
		return TargetDataAsset;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save package for asset: %s. ReturnCode: %d"), *FullAssetPath, ReturnCode);
		return nullptr;
	}

}

void UMPWidgetLibrary::CopyTextToClipboard(const FString& TextToCopy)
{
	FPlatformApplicationMisc::ClipboardCopy(*TextToCopy);
}

bool UMPWidgetLibrary::IsValidPackagePath(const FString& PackagePath)
{
	return FPackageName::IsValidObjectPath(PackagePath);
}

int32 UMPWidgetLibrary::SetupMaterialForBoneAnimation(
	const TArray<UMaterialInterface*>& MaterialsToProcess, 
	UMaterialFunctionMaterialLayer* BoneAnimationLayer, 
	UMaterialFunction* StandardFunction, UMaterialFunction* AttributeFunction, 
	FName LayerInputName, FName WPOInputName, FName WPOOutputName, FName NormalInputName, FName NormalOutputName,
	FName Attribute_LayerInputName, FName Attribute_MAInputName, FName Attribute_MAOutputName)
{
	// Input Validation
	if (!StandardFunction || !BoneAnimationLayer)
	{
		UE_LOG(LogTemp, Error, TEXT("Material Function and Layer is invalid."));
		return 0;
	}
	if (MaterialsToProcess.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Material is invalid."));
		return 0;
	}

	int32 SuccessCount = 0;
	FScopedTransaction Transaction(FText::FromString(TEXT("Batch Intercept Material Properties")));

	TSet<UMaterial*> ProcessedMaterials;

	const int32 NodeSpacingX = 300;

	// Loop through each material interface to process it.
	for (UMaterialInterface* MatInterface : MaterialsToProcess)
	{
		UMaterial* Material = MatInterface->GetMaterial();
		if (!Material) continue;

		if (ProcessedMaterials.Contains(Material))
		{
			continue;
		}

		UMaterialEditorOnlyData* EditorOnlyData = Material->GetEditorOnlyData();
		if (!EditorOnlyData)
		{
			UE_LOG(LogTemp, Error, TEXT("'%s' Fail To Get EditorOnlyData"), *Material->GetName());
			continue;
		}

		// Check if the material has already been processed by looking for our specific nodes.
		bool bAlreadyProcessed = false;
		for (const UMaterialExpression* Expression : EditorOnlyData->ExpressionCollection.Expressions)
		{
			if (!Expression) continue;

			if (const UMaterialExpressionMaterialFunctionCall* FunctionCallNode = Cast<const UMaterialExpressionMaterialFunctionCall>(Expression))
			{
				if (FunctionCallNode->MaterialFunction == StandardFunction || FunctionCallNode->MaterialFunction == AttributeFunction)
				{
					bAlreadyProcessed = true;
					break;
				}
			}

			else if (const UMaterialExpressionMaterialAttributeLayers* LayerNode = Cast<const UMaterialExpressionMaterialAttributeLayers>(Expression))
			{
				bAlreadyProcessed = true;
			}
		}

		if (bAlreadyProcessed)
		{
			UE_LOG(LogTemp, Warning, TEXT("Skipping material '%s' because it already contains a Material Attribute Layers node."), *Material->GetName());
			continue;
		}

		ProcessedMaterials.Add(Material);

		Material->Modify();
		EditorOnlyData->Modify();

		// Move the main material output node to the right to make space for the new nodes.
		const int32 OriginalOutputNodeX = Material->EditorX;
		const int32 OriginalOutputNodeY = Material->EditorY;

		const int32 LayerNodeX = OriginalOutputNodeX;
		const int32 LayerNodeY = OriginalOutputNodeY;
		const int32 FunctionNodeX = LayerNodeX + NodeSpacingX;
		const int32 FunctionNodeY = OriginalOutputNodeY;

		Material->EditorX = FunctionNodeX + NodeSpacingX * 2;

		// Create the Material Attribute Layers node, which will contain our bone animation layer.
		auto* LayerNode = Cast<UMaterialExpressionMaterialAttributeLayers>(UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionMaterialAttributeLayers::StaticClass()));
		LayerNode->MaterialExpressionEditorX = LayerNodeX;
		LayerNode->MaterialExpressionEditorY = LayerNodeY;

		FMaterialLayersFunctions NewLayers;
		NewLayers.AddDefaultBackgroundLayer();
		NewLayers.Layers[0] = BoneAnimationLayer;
		if (NewLayers.EditorOnly.LayerGuids.IsValidIndex(0))
		{
			NewLayers.EditorOnly.LayerGuids[0] = BoneAnimationLayer->StateId;
		}

		LayerNode->DefaultLayers = NewLayers;
		LayerNode->RebuildLayerGraph(true);

		// Handle the two distinct cases: Materials using Material Attributes vs. standard properties.
		if (Material->bUseMaterialAttributes)
		{
			UMaterialExpression* OriginalMAExpression = EditorOnlyData->MaterialAttributes.Expression;
			const int32 OriginalMAOutputIndex = EditorOnlyData->MaterialAttributes.OutputIndex;

			if (!OriginalMAExpression) continue;

			auto* FunctionCallNode = Cast<UMaterialExpressionMaterialFunctionCall>(UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionMaterialFunctionCall::StaticClass()));
			FunctionCallNode->SetMaterialFunction(AttributeFunction);
			FunctionCallNode->MaterialExpressionEditorX = FunctionNodeX;
			FunctionCallNode->MaterialExpressionEditorY = FunctionNodeY;

			UMaterialEditingLibrary::ConnectMaterialExpressions(LayerNode, TEXT(""), FunctionCallNode, Attribute_LayerInputName.ToString());

			const FName FromMAOutputName = OriginalMAExpression->Outputs.IsValidIndex(OriginalMAOutputIndex) ? OriginalMAExpression->Outputs[OriginalMAOutputIndex].OutputName : NAME_None;
			if (FromMAOutputName != NAME_None)
			{
				UMaterialEditingLibrary::ConnectMaterialExpressions(OriginalMAExpression, FromMAOutputName.ToString(), FunctionCallNode, Attribute_MAInputName.ToString());
			}
			UMaterialEditingLibrary::ConnectMaterialProperty(FunctionCallNode, Attribute_MAOutputName.ToString(), EMaterialProperty::MP_MaterialAttributes);
		}
		else
		{
			UMaterialExpression* OriginalWPOExpression = EditorOnlyData->WorldPositionOffset.Expression;
			const int32 OriginalWPOOutputIndex = EditorOnlyData->WorldPositionOffset.OutputIndex;

			UMaterialExpression* OriginalNormalExpression = EditorOnlyData->Normal.Expression;
			const int32 OriginalNormalOutputIndex = EditorOnlyData->Normal.OutputIndex;

			if (!OriginalWPOExpression && !OriginalNormalExpression)
			{
				UE_LOG(LogTemp, Log, TEXT("'%s'There are no WPO and Normal."), *Material->GetName());
				continue;
			}

			auto* FunctionCallNode = Cast<UMaterialExpressionMaterialFunctionCall>(UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionMaterialFunctionCall::StaticClass()));
			FunctionCallNode->SetMaterialFunction(StandardFunction);
			FunctionCallNode->MaterialExpressionEditorX = FunctionNodeX;
			FunctionCallNode->MaterialExpressionEditorY = FunctionNodeY;

			UMaterialEditingLibrary::ConnectMaterialExpressions(LayerNode, "", FunctionCallNode, LayerInputName.ToString());

			if (OriginalWPOExpression)
			{
				FName FromWPOOutputName = NAME_None;
				if (OriginalWPOExpression->Outputs.IsValidIndex(OriginalWPOOutputIndex))
				{
					FromWPOOutputName = OriginalWPOExpression->Outputs[OriginalWPOOutputIndex].OutputName;
				}

				FString FromWPOOutputString = FromWPOOutputName.ToString();
				if (FromWPOOutputString == TEXT("None"))
				{
					FromWPOOutputString = TEXT("");
				}

				UMaterialEditingLibrary::ConnectMaterialExpressions(OriginalWPOExpression, FromWPOOutputName.ToString(), FunctionCallNode, WPOInputName.ToString());			
			}
			UMaterialEditingLibrary::ConnectMaterialProperty(FunctionCallNode, WPOOutputName.ToString(), EMaterialProperty::MP_WorldPositionOffset);

			if (OriginalNormalExpression)
			{
				FName FromNormalOutputName = NAME_None;
				if (OriginalNormalExpression->Outputs.IsValidIndex(OriginalNormalOutputIndex))
				{
					FromNormalOutputName = OriginalNormalExpression->Outputs[OriginalNormalOutputIndex].OutputName;
				}

				FString FromNormalOutputString = FromNormalOutputName.ToString();
				if (FromNormalOutputString == TEXT("None"))
				{
					FromNormalOutputString = TEXT("");
				}

				UMaterialEditingLibrary::ConnectMaterialExpressions(OriginalNormalExpression, FromNormalOutputName.ToString(), FunctionCallNode, NormalInputName.ToString());
				
			}
			UMaterialEditingLibrary::ConnectMaterialProperty(FunctionCallNode, NormalOutputName.ToString(), EMaterialProperty::MP_Normal);
		}

		SuccessCount++;
	}

	if (SuccessCount > 0)
	{
		// Recompile all modified materials once after the loop.
		for (UMaterialInterface* MatInterface : MaterialsToProcess)
		{
			if (UMaterial* Material = MatInterface->GetMaterial())
			{
				Material->PostEditChange();
				Material->MarkPackageDirty();
				UMaterialEditingLibrary::RecompileMaterial(Material);
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Successfully processed %d materials."), SuccessCount);
	}
	else
	{
		// If no work was done, cancel the transaction to avoid an empty Undo entry.
		Transaction.Cancel();
		UE_LOG(LogTemp, Warning, TEXT("No materials could be processed."));
	}

	return SuccessCount;
}

USkeletalMesh* UMPWidgetLibrary::DuplicateSkeletalMeshWithMaterials(USkeletalMesh* SourceMesh, const FString& TargetFolderPath, const FString& NewBaseAssetName, const FString& Suffix)
{
	// Input Validation
	if (!SourceMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Source Skeletal Mesh is null."));
		return nullptr;
	}
	if (!FPackageName::IsValidObjectPath(TargetFolderPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Target folder path must start with '/Game/'. Path: %s"), *TargetFolderPath);
		return nullptr;
	}
	if (NewBaseAssetName.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("New Base Asset Name cannot be empty."));
		return nullptr;
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();

	const FString FinalAssetName = NewBaseAssetName + Suffix;
	const FString FinalFolderPath = FPaths::Combine(TargetFolderPath, FinalAssetName);

	// Collect all unique material dependencies from the source mesh
	TSet<UMaterialInterface*> MaterialsToDuplicate;
	for (const FSkeletalMaterial& SkelMaterial : SourceMesh->GetMaterials())
	{
		CollectMaterialDependencies(SkelMaterial.MaterialInterface, MaterialsToDuplicate);
	}

	// Duplicate all collected assets (materials and the mesh)
	// TMap<Source, Copy>
	TMap<UObject*, UObject*> OriginalToDuplicateMap;

	// Duplicate all materials and material instances.
	for (UMaterialInterface* Material : MaterialsToDuplicate)
	{
		if (!Material) continue;

		const FString NewMaterialName = Material->GetName() + Suffix;

		UObject* DuplicatedMaterial = AssetTools.DuplicateAsset(NewMaterialName, FinalFolderPath, Material);
		if (DuplicatedMaterial)
		{
			OriginalToDuplicateMap.Add(Material, DuplicatedMaterial);
		}
	}

	// Duplicate the Skeletal Mesh itself.
	USkeletalMesh* DuplicatedSkeletalMesh = Cast<USkeletalMesh>(AssetTools.DuplicateAsset(FinalAssetName, FinalFolderPath, SourceMesh));

	if (!DuplicatedSkeletalMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to duplicate Skeletal Mesh."));
		return nullptr;
	}
	OriginalToDuplicateMap.Add(SourceMesh, DuplicatedSkeletalMesh);

	// Re-parent the duplicated Material Instances
	for (auto const& Pair : OriginalToDuplicateMap)
	{
		UMaterialInstanceConstant* DupInst = Cast<UMaterialInstanceConstant>(Pair.Value);
		if (DupInst)
		{
			UMaterialInstance* OriginalInst = Cast<UMaterialInstance>(Pair.Key);
			if (OriginalInst && OriginalInst->Parent)
			{
				if (UObject** FoundDupParent = OriginalToDuplicateMap.Find(OriginalInst->Parent))
				{
					UMaterialInterface* NewParent = Cast<UMaterialInterface>(*FoundDupParent);
					if (NewParent)
					{
						DupInst->SetParentEditorOnly(NewParent);
						DupInst->PostEditChange();
					}
				}
			}
		}
	}

	// Assign the new materials to the new Skeletal Mesh
	TArray<FSkeletalMaterial>& TargetMaterials = DuplicatedSkeletalMesh->GetMaterials();
	bool bMaterialsChanged = false;
	for (int32 i = 0; i < TargetMaterials.Num(); ++i)
	{
		UMaterialInterface* OriginalMaterial = SourceMesh->GetMaterials()[i].MaterialInterface;
		if (OriginalMaterial)
		{
			// Assign the duplicated material if it exists in the map
			if (UObject** FoundDupMaterial = OriginalToDuplicateMap.Find(OriginalMaterial))
			{
				UMaterialInterface* NewMaterial = Cast<UMaterialInterface>(*FoundDupMaterial);
				if (NewMaterial && TargetMaterials[i].MaterialInterface != NewMaterial)
				{
					TargetMaterials[i].MaterialInterface = NewMaterial;
					bMaterialsChanged = true;
				}
			}
		}
	}

	if (bMaterialsChanged)
	{
		DuplicatedSkeletalMesh->PostEditChange();
		DuplicatedSkeletalMesh->MarkPackageDirty();
	}

	for (auto const& Pair : OriginalToDuplicateMap)
	{
		if (Pair.Value)
		{
			Pair.Value->MarkPackageDirty();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Successfully duplicated Skeletal Mesh '%s' to '%s' with all material dependencies."), *SourceMesh->GetName(), *DuplicatedSkeletalMesh->GetName());

	return DuplicatedSkeletalMesh;
}

bool UMPWidgetLibrary::IsValidForSetupBoneAnimation(UMaterialInterface* MaterialToCheck, UMaterialFunctionMaterialLayer* LayerToCheck, UMaterialFunction* StandardFunctionToCheck, UMaterialFunction* AttributeFunctionToCheck)
{
	if (!MaterialToCheck || !LayerToCheck || !StandardFunctionToCheck || !AttributeFunctionToCheck)
	{
		return false;
	}

	UMaterial* Material = MaterialToCheck->GetMaterial();
	if (!Material)
	{
		return false;
	}

	UMaterialEditorOnlyData* EditorOnlyData = Material->GetEditorOnlyData();
	if (!EditorOnlyData)
	{
		return false;
	}

	for (const UMaterialExpression* Expression : EditorOnlyData->ExpressionCollection.Expressions)
	{
		if (!Expression) continue;

		// Check 1: See if a function call node using one of our target functions already exists.
		if (const auto* FunctionCallNode = Cast<const UMaterialExpressionMaterialFunctionCall>(Expression))
		{
			if (FunctionCallNode->MaterialFunction == StandardFunctionToCheck ||
				FunctionCallNode->MaterialFunction == AttributeFunctionToCheck)
			{
				// Found a matching function call, so it's processed.
				return false;
			}
		}
		// Check 2: See if a layer node already contains our target layer asset.
		else if (const auto* LayerNode = Cast<const UMaterialExpressionMaterialAttributeLayers>(Expression))
		{
			for (const UMaterialFunctionInterface* Layer : LayerNode->DefaultLayers.Layers)
			{
				return false;
			}
		}
	}

	return true;
}

bool UMPWidgetLibrary::IsBatchValidForSetupBoneAnimation(TArray<UMaterialInterface*> MaterialToCheck, UMaterialFunctionMaterialLayer* LayerToCheck, UMaterialFunction* StandardFunctionToCheck, UMaterialFunction* AttributeFunctionToCheck)
{
	if (MaterialToCheck.Num() == 0 || !LayerToCheck || !StandardFunctionToCheck || !AttributeFunctionToCheck)
	{
		return false;
	}

	for (UMaterialInterface* Material : MaterialToCheck)
	{
		if (!IsValidForSetupBoneAnimation(Material, LayerToCheck, StandardFunctionToCheck, AttributeFunctionToCheck))
		{
			return false;
		}
	}

	return true;
}

void UMPWidgetLibrary::UpdateMPSpawnerNiagara()
{
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		return ;
	}

	for (TActorIterator<AMPSpawner> It(World); It; ++It)
	{
		AMPSpawner* TargetActor = *It;
		if (TargetActor)
		{
			TargetActor->UpdateNiagaraComponent();
		}
	}


	return ;
}

