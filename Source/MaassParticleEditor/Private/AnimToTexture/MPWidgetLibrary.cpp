// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "AnimToTexture/MPWidgetLibrary.h"
#include "AnimToTexture/MPAnimToTextureDataAsset.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "FileHelpers.h"
#include "Modules/ModuleManager.h"
#include "GenericPlatform/GenericPlatformApplicationMisc.h"
#include "HAL/PlatformApplicationMisc.h"


UMPAnimToTextureDataAsset* UMPWidgetLibrary::CreateAndProcessDataAsset_Library(const FString& PackagePath, const FString& AssetName, int32 TextureMaxWidth, int32 TextureMaxHeight, USkeletalMesh* SkeletalMesh, const TArray<UAnimSequence*>& SelectedAnims)
{
    if (PackagePath.IsEmpty() || !PackagePath.StartsWith("/Game/"))
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

    const FString Prefix = TEXT("DA_");
    const FString Suffix = TEXT("_BAT");
    
    const FString FinalAssetName = FString::Printf(TEXT("%s%s%s"), *Prefix, *AssetName, *Suffix);

    const FString FullAssetPath = PackagePath + "/" + FinalAssetName;

    ////
    UMPAnimToTextureDataAsset* TargetDataAsset = nullptr;

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

    
	TargetDataAsset->MaxWidth = TextureMaxWidth;
	TargetDataAsset->MaxHeight = TextureMaxHeight;
    TargetDataAsset->SkeletalMesh = SkeletalMesh;
    TargetDataAsset->AnimSequences.Empty();
    
    for (UAnimSequence* Anim : SelectedAnims)
    {
        if (IsValid(Anim))
        {
            FAnimToTextureAnimSequenceInfo NewInfo;
            NewInfo.AnimSequence = Anim;
            TargetDataAsset->AnimSequences.Add(NewInfo);
        }
    }

    TargetDataAsset->MarkPackageDirty();

    TArray<UPackage*> PackagesToSave;
    PackagesToSave.Add(TargetDataAsset->GetPackage());

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
