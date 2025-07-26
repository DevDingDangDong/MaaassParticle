// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "UI/MPSpawnerDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "PropertyHandle.h"


#define LOCTEXT_NAMESPACE "MPSpawnerDetails"

FMPSpawnerDetails::FMPSpawnerDetails()
{

}

FMPSpawnerDetails::~FMPSpawnerDetails()
{

}

/**
 * Creates and returns a shared reference to a new FEasyCrowdSpawnerDetails.
 */
TSharedRef<IDetailCustomization> FMPSpawnerDetails::MakeInstance()
{
    return MakeShareable(new FMPSpawnerDetails());
}

/**
 * Sets up property change callbacks, retrieves the AEasyCrowdSpawner being customized,
 * and updates the visibility of dependent properties in the details panel.
 */
void FMPSpawnerDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    DetailBuilder.EditCategory("Transform");

    static const FText MaaassParticleDisplayName = LOCTEXT("MaaassParticleCategory", "MaaassParticle");
    IDetailCategoryBuilder& MaaassParticleCategory =
        DetailBuilder.EditCategory(
            TEXT("MaaassParticle"),         
            MaaassParticleDisplayName,          
            ECategoryPriority::Variable    
        );

    static const FText MPSpawnerDataAssetDisplayName = LOCTEXT("MPSpawnerDataAssetCategory", "MPSpawnerDataAsset");
    IDetailCategoryBuilder& MPSpawnerDataAssetCategory =
        DetailBuilder.EditCategory(
            TEXT("MaaassParticle|MPSpawnerDataAsset"),
            MPSpawnerDataAssetDisplayName,
            ECategoryPriority::Variable   
        );

    MaaassParticleCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, MPSpawnerDataAsset));

    TSharedRef<IPropertyHandle> AssetHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, MPSpawnerDataAsset));
    AssetHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([&DetailBuilder]()
        {
            DetailBuilder.ForceRefreshDetails();
        }));

    // Get the actor instance
    TArray<TWeakObjectPtr<UObject>> Objects;
    DetailBuilder.GetObjectsBeingCustomized(Objects);
    if (Objects.Num() > 0)
    {
        Spawner = Cast<AMPSpawner>(Objects[0].Get());
    }

    // Show dependent properties only if asset is set
    if (Spawner.IsValid() && Spawner->MPSpawnerDataAsset)
    {
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, AnimToTextureDataAsset));
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, CrowdNiagaraSystem));
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, EntityConfigAsset));
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, DefaultAnimState));
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, SpawnDataGenerator));
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, SpawnCount));
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, LoopBehavior));
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, LoopCount));
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, LoopDuration));   
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, KillParticleOnLifeHasElapsed));
        MPSpawnerDataAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, ParticleLifeTime));
    }
    else
    {
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, AnimToTextureDataAsset));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, CrowdNiagaraSystem));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, EntityConfigAsset));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, DefaultAnimState));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, SpawnDataGenerator));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, SpawnCount));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, LoopBehavior));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, LoopCount));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, LoopDuration));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, KillParticleOnLifeHasElapsed));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, ParticleLifeTime));
    }
}

#undef LOCTEXT_NAMESPACE