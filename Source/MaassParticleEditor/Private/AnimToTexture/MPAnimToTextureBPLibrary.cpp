// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "AnimToTexture/MPAnimToTextureBPLibrary.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "MaterialEditingLibrary.h"
#include "MeshUtilities.h"
#include "RawMesh.h"
#include "StaticMeshEditorSubsystem.h"
#include "AnimToTextureEditor/Public/AnimToTextureBPLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "MaaassParticle/Public/AnimToTexture//MPAnimToTextureDataAsset.h"

#define LOCTEXT_NAMESPACE "MPAnimToTextureEditor"

UStaticMesh* UMPAnimToTextureBPLibrary::ConvertSkeletalMeshToStaticMeshWithLODs(UMPAnimToTextureDataAsset* DataAsset, TArray<UMaterialInterface*> Materials, const FString PackagePath, const FString AssetName)
{
    // Validate input data asset and skeletal mesh
    check(DataAsset->SkeletalMesh);

    // Generate the full asset name with prefix
    FString FullAssetName = TEXT("SM_") + AssetName;
    const FString FullPath = FPaths::Combine(PackagePath, FullAssetName);

    // Validate the generated path
    if (FullPath.IsEmpty() || !FPackageName::IsValidObjectPath(FullPath))
    {
        return nullptr;
    }
    
    // Get editor world for spawning temporary actor
    check(GEditor);
    UWorld* World = GEditor->GetEditorWorldContext().World();
    check(World);
    
    // Create temporary actor to hold the skeletal mesh component
    AActor* Actor = World->SpawnActor<AActor>();
    check(Actor);

    // Prepare skeletal materials array from input materials
    TArray<FSkeletalMaterial> SkeletalMaterials;
    SkeletalMaterials.SetNum(Materials.Num());
    for (int MaterialIndex = 0; MaterialIndex < Materials.Num(); MaterialIndex++)
    {
        SkeletalMaterials[MaterialIndex].MaterialInterface = Materials[MaterialIndex];
    }

    // Create a working copy of the skeletal mesh to avoid modifying the original
    USkeletalMesh* MeshCopy = DuplicateObject<USkeletalMesh>(DataAsset->SkeletalMesh.Get(), GetTransientPackage());
    MeshCopy->SetMaterials(SkeletalMaterials);
    
    // Create and configure skeletal mesh component
    USkeletalMeshComponent* MeshComponent = NewObject<USkeletalMeshComponent>(Actor);
    MeshComponent->RegisterComponent();
    MeshComponent->SetSkeletalMesh(MeshCopy);
    
    // Prepare mesh components array for conversion
    TArray<UMeshComponent*> MeshComponents = { MeshComponent };

    UStaticMesh* OutStaticMesh = nullptr;
    
    // Check if static mesh already exists, if not create new one
    if (!FPackageName::DoesPackageExist(FullPath))
    {
        // Create new static mesh using mesh utilities
        IMeshUtilities& MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshUtilities>("MeshUtilities");
        OutStaticMesh = MeshUtilities.ConvertMeshesToStaticMesh(MeshComponents, FTransform::Identity, FullPath);
    }
    else
    {
        // Load existing static mesh
        OutStaticMesh = LoadObject<UStaticMesh>(nullptr, *FullPath);
    }

    if (OutStaticMesh)
    {
        // Create temporary mesh for extracting LOD data
        UPackage* TransientPackage = GetTransientPackage();
        
        IMeshUtilities& MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshUtilities>("MeshUtilities");
        UStaticMesh* TempMesh = MeshUtilities.ConvertMeshesToStaticMesh(MeshComponents, FTransform::Identity, TransientPackage->GetPathName());
        
        // Mark temporary mesh as transactional for undo/redo support
        TempMesh->SetFlags(RF_Transactional);
        
        // Copy all LOD levels from temporary mesh to output mesh
        const int32 NumSourceModels = TempMesh->GetNumSourceModels();
        OutStaticMesh->SetNumSourceModels(NumSourceModels);

        // Process each LOD level
        for (int32 Index = 0; Index < NumSourceModels; ++Index)
        {
            // Extract raw mesh data from temporary mesh
            FRawMesh RawMesh;
            TempMesh->GetSourceModel(Index).LoadRawMesh(RawMesh);
            
            // Save raw mesh data to output mesh and configure lightmap settings
            OutStaticMesh->GetSourceModel(Index).SaveRawMesh(RawMesh);
            OutStaticMesh->GetSourceModel(Index).BuildSettings.DstLightmapIndex = 2;
        };
        
        // Copy material assignments from temporary mesh
        const TArray<FStaticMaterial>& Materials = TempMesh->GetStaticMaterials();
        OutStaticMesh->SetStaticMaterials(Materials);

        // Build the static mesh and mark as dirty for saving
        OutStaticMesh->Build(false);
        OutStaticMesh->MarkPackageDirty();

        // Update data asset reference to the created static mesh
        DataAsset->StaticMesh = OutStaticMesh;
    }
    
    // Clean up temporary components and actor
    MeshComponent->UnregisterComponent();
    MeshComponent->DestroyComponent();
    Actor->Destroy();

    return OutStaticMesh;
}

UStaticMesh* UMPAnimToTextureBPLibrary::SetLightMapWithLODs(UStaticMesh* StaticMesh, const int32 LightmapIndex, bool bGenerateLightmapUVs)
{
    // Validate input static mesh
    check(StaticMesh);

    // Get total number of LOD levels
    int32 LODNum = StaticMesh->GetNumLODs();

    // Configure lightmap settings for each LOD level
    for (int LODIndex = 0; LODIndex < LODNum; ++LODIndex)
    {
        // Ensure sufficient UV channels exist for the specified lightmap index
        while (StaticMesh->GetNumUVChannels(LODIndex) <= LightmapIndex)
        {
            StaticMesh->AddUVChannel(LODIndex);
        }
        
        // Configure build settings for this LOD level
        FStaticMeshSourceModel& SourceModel = StaticMesh->GetSourceModel(LODIndex);
        SourceModel.BuildSettings.bGenerateLightmapUVs = bGenerateLightmapUVs;
        SourceModel.BuildSettings.DstLightmapIndex = LightmapIndex;
    }
    
    // Set lightmap coordinate index for the entire mesh
    StaticMesh->SetLightMapCoordinateIndex(LightmapIndex);
    
    // Rebuild mesh with new settings and mark as modified
    StaticMesh->Build(false);
    StaticMesh->PostEditChange();
    StaticMesh->MarkPackageDirty();

    return StaticMesh;
}

bool UMPAnimToTextureBPLibrary::CreateWeightTextureWithLODs(UMPAnimToTextureDataAsset* DataAsset, const FString& PackagePath, const FString& AssetName)
{
    // Validate data asset and its static mesh
    UStaticMesh* OriginStaticMesh = DataAsset->GetStaticMesh();
    if (!OriginStaticMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("DataAsset does not have a valid StaticMesh"));
        return false;
    }

    // Ensure animation sequences exist
    if (DataAsset->AnimSequences.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("DataAsset does not have animation"));
        return false;
    }
    
    // Create bone position texture if it doesn't exist
    if (!DataAsset->BonePositionTexture)
    {
        FString FullAssetName = TEXT("T_Bone_Position_") + AssetName;
        TArray<uint16> BlackPixel;
        BlackPixel.AddZeroed();
        UTexture2D* PositionTexture = CreatePersistentTexture(PackagePath, FullAssetName, 256, 256, BlackPixel);
        DataAsset->BonePositionTexture = PositionTexture;
    }

    // Create bone rotation texture if it doesn't exist
    if (!DataAsset->BoneRotationTexture)
    {
        FString FullAssetName = TEXT("T_Bone_Rotation_") + AssetName;
        TArray<uint16> BlackPixel;
        BlackPixel.AddZeroed();
        UTexture2D* RotationTexture = CreatePersistentTexture(PackagePath, FullAssetName, 256, 256, BlackPixel);
        DataAsset->BoneRotationTexture = RotationTexture;
    }
    
    // Initialize LOD info array to match static mesh LOD count
    DataAsset->LODInfos.Empty();
    DataAsset->LODInfos.Reserve(OriginStaticMesh->GetNumLODs());
    
    // Process each LOD level to create individual weight textures
    for (int LODIndex = 0; LODIndex < OriginStaticMesh->GetNumLODs(); LODIndex++)
    {
        // Set current LOD indices for animation texture generation
        DataAsset->SkeletalLODIndex = LODIndex;
        DataAsset->StaticLODIndex = LODIndex;
        
        // Create unique weight texture name for this LOD
        FString FullAssetName = FString::Printf(TEXT("T_Bone_Weight_%s_LOD%d"), GetData(AssetName), LODIndex);
        TArray<uint16> BlackPixel;
        BlackPixel.AddZeroed();
        UTexture2D* WeightTexture = CreatePersistentTexture(PackagePath, FullAssetName, 256, 256, BlackPixel);
        DataAsset->BoneWeightTexture = WeightTexture;

        // Generate animation texture data for this LOD
        UAnimToTextureBPLibrary::AnimationToTexture(DataAsset);
        
        // Store the generated weight texture in LOD info
        if (DataAsset->BoneWeightTexture)
        {
            FMPAnimToTextureLODInfo LODInfo;
            LODInfo.Texture = DataAsset->BoneWeightTexture.Get();
            DataAsset->LODInfos.Insert(LODInfo, LODIndex);
            UE_LOG(LogTemp, Log, TEXT("Successfully created weight texture for LOD %d: %s"), LODIndex, *DataAsset->BoneWeightTexture->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create persistent texture for LOD %d"), LODIndex);
        }
    }

    // Mark all generated textures as dirty for saving
    DataAsset->BonePositionTexture->MarkPackageDirty();
    DataAsset->BoneRotationTexture->MarkPackageDirty();
    DataAsset->BoneWeightTexture->MarkPackageDirty();
    
    // Update static mesh reference in data asset
    DataAsset->StaticMesh = OriginStaticMesh;
    
    return true;
}

bool UMPAnimToTextureBPLibrary::CreateMaterialInstanceWithLODs(UMPAnimToTextureDataAsset* DataAsset, const FString& PackagePath,
    const FString& AssetName)
{
    // Validate input data asset
    if (!DataAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("DataAsset is NULL"));
        return false;
    }
    
    // Validate static mesh in data asset
    if (!DataAsset->StaticMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("DataAsset does not have a valid StaticMesh"));
        return false;
    }

#if WITH_EDITOR
    UStaticMesh* StaticMesh = DataAsset->StaticMesh.Get();
    DataAsset->bAutoPlay = false;
    
    // Validate render data availability
    if (!StaticMesh->GetRenderData())
    {
        UE_LOG(LogTemp, Error, TEXT("StaticMesh does not have valid RenderData"));
        return false;
    }

    // Backup original material information
    TArray<FStaticMaterial> OriginalStaticMaterials = StaticMesh->GetStaticMaterials();
    TArray<FStaticMaterial> NewStaticMaterials;
    
    int32 LODNum = StaticMesh->GetNumLODs();
    int32 MaterialIndexCounter = 0;
    
    // Store section to material index mapping for each LOD
    TArray<TArray<int32>> LODSectionToMaterialMap;
    LODSectionToMaterialMap.SetNum(LODNum);
    
    // Collect original section material index information
    for (int32 LODIndex = 0; LODIndex < LODNum; ++LODIndex)
    {
        const FStaticMeshLODResources& LODResource = StaticMesh->GetRenderData()->LODResources[LODIndex];
        int32 SectionNum = LODResource.Sections.Num();
        
        LODSectionToMaterialMap[LODIndex].SetNum(SectionNum);
        
        for (int32 SectionIndex = 0; SectionIndex < SectionNum; ++SectionIndex)
        {
            int32 OriginalMaterialIndex = LODResource.Sections[SectionIndex].MaterialIndex;
            LODSectionToMaterialMap[LODIndex][SectionIndex] = OriginalMaterialIndex;
        }
    }
    
    // Process each LOD level individually
    for (int32 LODIndex = 0; LODIndex < LODNum; ++LODIndex)
    {
        // Get LOD resource data
        const FStaticMeshLODResources& LODResource = StaticMesh->GetRenderData()->LODResources[LODIndex];
        int32 SectionNum = LODResource.Sections.Num();
        
        UE_LOG(LogTemp, Log, TEXT("Processing LOD %d with %d sections"), LODIndex, SectionNum);
        
        // Validate LOD info and weight texture availability
        if (!DataAsset->LODInfos.IsValidIndex(LODIndex) || !DataAsset->LODInfos[LODIndex].Texture)
        {
            UE_LOG(LogTemp, Error, TEXT("DataAsset does not have a valid Weight Texture for LOD %d"), LODIndex);
            continue;
        }
        
        // Set current weight texture and clear existing material instances
        DataAsset->BoneWeightTexture = DataAsset->LODInfos[LODIndex].Texture;
        DataAsset->LODInfos[LODIndex].MaterialInstances.Empty();
        
        // Create material instance for each section in this LOD
        for (int32 SectionIndex = 0; SectionIndex < SectionNum; ++SectionIndex)
        {
            // Get original material index for this section
            int32 OriginalMaterialIndex = LODSectionToMaterialMap[LODIndex][SectionIndex];
            
            // Generate material slot name (use original material name if available)
            FString MaterialSlotName = TEXT("DefaultMaterial");
            if (OriginalStaticMaterials.IsValidIndex(OriginalMaterialIndex))
            {
                MaterialSlotName = OriginalStaticMaterials[OriginalMaterialIndex].MaterialSlotName.ToString();
            }
            
            // Create unique material instance name
            FString FullAssetName = FString::Printf(TEXT("MI_%s_%s_LOD%d_Section%d"), 
                                                   *AssetName, *MaterialSlotName, LODIndex, SectionIndex);
            
            // Find base material from original section material
            UMaterial* BaseMaterial = nullptr;
            if (OriginalStaticMaterials.IsValidIndex(OriginalMaterialIndex) && 
                OriginalStaticMaterials[OriginalMaterialIndex].MaterialInterface)
            {
                BaseMaterial = Cast<UMaterial>(OriginalStaticMaterials[OriginalMaterialIndex].MaterialInterface);
                if (!BaseMaterial)
                {
                    // Handle material instance case - get parent material
                    if (UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(OriginalStaticMaterials[OriginalMaterialIndex].MaterialInterface))
                    {
                        BaseMaterial = MaterialInstance->GetMaterial();
                    }
                }
            }
            
            // Use first material as fallback if base material not found
            if (!BaseMaterial && OriginalStaticMaterials.Num() > 0)
            {
                UE_LOG(LogTemp, Warning, TEXT("Could not find material for index %d, using first material as fallback"), OriginalMaterialIndex);
                
                BaseMaterial = Cast<UMaterial>(OriginalStaticMaterials[0].MaterialInterface);
                if (!BaseMaterial)
                {
                    if (UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(OriginalStaticMaterials[0].MaterialInterface))
                    {
                        BaseMaterial = MaterialInstance->GetMaterial();
                    }
                }
            }
            
            // Skip section if no base material found
            if (!BaseMaterial)
            {
                UE_LOG(LogTemp, Warning, TEXT("Could not find base material for LOD %d Section %d (Original Material Index: %d)"), 
                       LODIndex, SectionIndex, OriginalMaterialIndex);
                continue;
            }
            
            // Create material instance constant
            UMaterialInstanceConstant* LODMIC = CreateMaterialInstance(PackagePath, FullAssetName, BaseMaterial);
            if (!LODMIC)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create MaterialInstance for LOD %d Section %d"), LODIndex, SectionIndex);
                continue;
            }
            
            UMaterialEditingLibrary::SetMaterialInstanceStaticSwitchParameterValue(LODMIC, TEXT("UseDynamicParameters"), true, LayerParameter);
            
            DataAsset->LODInfos[LODIndex].MaterialInstances.Add(LODMIC);
            
            // Create static material structure preserving original information
            FStaticMaterial NewStaticMaterial;
            NewStaticMaterial.MaterialInterface = LODMIC;
            NewStaticMaterial.MaterialSlotName = *FString::Printf(TEXT("%s_LOD%d_Sec%d"), *MaterialSlotName, LODIndex, SectionIndex);
            NewStaticMaterial.ImportedMaterialSlotName = *FString::Printf(TEXT("%s_LOD%d_Sec%d"), *MaterialSlotName, LODIndex, SectionIndex);
            
            // Copy additional properties from original material if available
            if (OriginalStaticMaterials.IsValidIndex(OriginalMaterialIndex))
            {
                // Copy UV channel data and other properties
                NewStaticMaterial.UVChannelData = OriginalStaticMaterials[OriginalMaterialIndex].UVChannelData;
            }
            
            // Update material instance with data asset parameters
            UAnimToTextureBPLibrary::UpdateMaterialInstanceFromDataAsset(DataAsset, LODMIC, LayerParameter);
            
            NewStaticMaterials.Add(NewStaticMaterial);
            MaterialIndexCounter++;

            LODMIC->PostEditChange();
        }
    }
    
    // Apply new material array to static mesh using UStaticMeshEditorSubsystem
    if (NewStaticMaterials.Num() > 0)
    {
        // Get StaticMeshEditorSubsystem
        UStaticMeshEditorSubsystem* StaticMeshEditorSubsystem = 
            GEditor->GetEditorSubsystem<UStaticMeshEditorSubsystem>();
        
        if (!StaticMeshEditorSubsystem)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get StaticMeshEditorSubsystem"));
            return false;
        }

        // First, set the materials to the static mesh
        StaticMesh->SetStaticMaterials(NewStaticMaterials);
        
        // Apply materials to each LOD section using the subsystem
        MaterialIndexCounter = 0;
        bool bAllAssignmentsSuccessful = true;
        
        for (int32 LODIndex = 0; LODIndex < LODNum; ++LODIndex)
        {
            // Get LOD section count safely
            int32 SectionCount = 0;
            if (StaticMesh->GetRenderData() && 
                StaticMesh->GetRenderData()->LODResources.IsValidIndex(LODIndex))
            {
                SectionCount = StaticMesh->GetRenderData()->LODResources[LODIndex].Sections.Num();
            }
            
            for (int32 SectionIndex = 0; SectionIndex < SectionCount; ++SectionIndex)
            {
                // Ensure we don't exceed the material array bounds
                if (MaterialIndexCounter >= NewStaticMaterials.Num())
                {
                    UE_LOG(LogTemp, Warning, TEXT("MaterialIndexCounter exceeded NewStaticMaterials array size"));
                    break;
                }
                
                // Get the material to assign
                UMaterialInterface* MaterialToAssign = NewStaticMaterials[MaterialIndexCounter].MaterialInterface;
                
                if (MaterialToAssign)
                {
                    // Use StaticMeshEditorSubsystem to assign material to specific LOD section
                    StaticMeshEditorSubsystem->SetLODMaterialSlot(
                        StaticMesh, 
                        MaterialIndexCounter, 
                        LODIndex, 
                        SectionIndex
                    );
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("MaterialToAssign is null for MaterialIndexCounter %d"), 
                           MaterialIndexCounter);
                }
                
                MaterialIndexCounter++;
            }
        }
        
        // Apply changes and mark as dirty
        StaticMesh->PostEditChange();
        StaticMesh->MarkPackageDirty();
        
        // Force update the static mesh
        if (StaticMeshEditorSubsystem)
        {
            // Rebuild the static mesh to ensure changes are applied
            StaticMeshEditorSubsystem->ReimportAllCustomLODs(StaticMesh);
        }
        
        if (bAllAssignmentsSuccessful)
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully created and assigned %d MaterialInstances for StaticMesh LODs (Original Materials: %d)"), 
                   NewStaticMaterials.Num(), OriginalStaticMaterials.Num());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Some material assignments failed, but %d MaterialInstances were created"), 
                   NewStaticMaterials.Num());
        }
    }

    return true;
    
#else
    UE_LOG(LogTemp, Error, TEXT("This function can only be used in Editor builds"));
    return false;
#endif
}

UTexture2D* UMPAnimToTextureBPLibrary::CreatePersistentTexture(const FString& PackagePath, const FString& AssetName, int32 Width,
                                                      int32 Height, const TArray<uint16>& PixelData)
{
#if WITH_EDITOR
    // Generate full asset path
    FString FullPath = FPaths::Combine(PackagePath, AssetName);
    
    UE_LOG(LogTemp, Log, TEXT("Attempting to create texture at: %s"), *FullPath);
    
    // Check for existing texture first
    UTexture2D* ExistingTexture = LoadObject<UTexture2D>(nullptr, *FullPath);
    if (ExistingTexture)
    {
        UE_LOG(LogTemp, Log, TEXT("Found existing texture at: %s"), *FullPath);
        return ExistingTexture;
    }

    // Create package for the new texture
    UPackage* Package = CreatePackage(*FullPath);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *FullPath);
        return nullptr;
    }

    // Create new texture object
    UTexture2D* NewTexture = NewObject<UTexture2D>(Package, FName(*AssetName), RF_Public | RF_Standalone);
    if (!NewTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create texture object"));
        return nullptr;
    }
    
    // Initialize texture source data with 16-bit RGBA format
    NewTexture->Source.Init(Width, Height, 1, 1, TSF_RGBA16, reinterpret_cast<const uint8*>(PixelData.GetData()));
    
    // Configure texture settings for weight data
    NewTexture->SRGB = false;  // Disable SRGB for weight textures
    NewTexture->CompressionSettings = TC_HDR;  // Use HDR compression
    NewTexture->MipGenSettings = TMGS_NoMipmaps;  // Disable mipmap generation
    NewTexture->Filter = TF_Nearest;  // Use nearest filtering for pixel accuracy
    NewTexture->NeverStream = true;  // Prevent streaming to ensure data availability
    
    // Update texture resource
    NewTexture->UpdateResource();
    
    // Mark package and texture as dirty
    Package->MarkPackageDirty();
    NewTexture->MarkPackageDirty();
    
    // Save package to disk
    FString PackageFileName = FPackageName::LongPackageNameToFilename(FullPath, FPackageName::GetAssetPackageExtension());
    
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.bForceByteSwapping = false;
    SaveArgs.bWarnOfLongFilename = false;
    
    bool bSaved = UPackage::SavePackage(Package, NewTexture, *PackageFileName, SaveArgs);
    
    if (bSaved)
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully saved texture: %s"), *PackageFileName);
        
        // Post edit change after successful save
        NewTexture->PostEditChange();
        
        // Register with asset registry
        FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        
        // Perform immediate file scan
        AssetRegistryModule.Get().ScanFilesSynchronous({PackageFileName});
        
        // Register asset creation
        FAssetData AssetData(NewTexture);
        AssetRegistryModule.Get().AssetCreated(NewTexture);
        
        // Synchronize content browser on next tick
        FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
        AsyncTask(ENamedThreads::GameThread, [AssetData, &ContentBrowserModule]()
        {
            ContentBrowserModule.Get().SyncBrowserToAssets({AssetData});
        });
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save texture: %s"), *PackageFileName);
        return nullptr;
    }
    
    return NewTexture;
    
#else
    return nullptr;
#endif
}

UMaterialInstanceConstant* UMPAnimToTextureBPLibrary::CreateMaterialInstance(const FString& PackagePath,
    const FString& AssetName, UMaterialInterface* SourceMaterial)
{
    // Validate source material
    if (!SourceMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("SourceMaterial is null"));
        return nullptr;
    }
    
    // Generate full asset path
    FString FullAssetPath = FPaths::Combine(PackagePath, AssetName);
    
    // Check if material instance already exists
    UMaterialInstanceConstant* ExistingMaterialInstance = LoadObject<UMaterialInstanceConstant>(nullptr, *FullAssetPath);
    if (ExistingMaterialInstance)
    {
        UE_LOG(LogTemp, Log, TEXT("Material Instance already exists: %s"), *FullAssetPath);
        return ExistingMaterialInstance;
    }
    
    // Create or get package
    UPackage* Package = CreatePackage(*PackagePath);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *PackagePath);
        return nullptr;
    }
    
    // Create material instance constant object
    UMaterialInstanceConstant* NewMaterialInstance = NewObject<UMaterialInstanceConstant>(
        Package, 
        UMaterialInstanceConstant::StaticClass(),
        *AssetName, 
        RF_Public | RF_Standalone | RF_Transactional
    );
    
    if (!NewMaterialInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create MaterialInstanceConstant"));
        return nullptr;
    }
    
    // Set parent material relationship
    NewMaterialInstance->SetParentEditorOnly(SourceMaterial);
    
    // Trigger editor update
    NewMaterialInstance->PostEditChange();
    
    // Register with asset registry
    FAssetRegistryModule::AssetCreated(NewMaterialInstance);
    
    // Mark package as dirty for saving
    Package->MarkPackageDirty();
    
    UE_LOG(LogTemp, Log, TEXT("Created new Material Instance: %s"), *FullAssetPath);
    
    return NewMaterialInstance;
}

#undef LOCTEXT_NAMESPACE