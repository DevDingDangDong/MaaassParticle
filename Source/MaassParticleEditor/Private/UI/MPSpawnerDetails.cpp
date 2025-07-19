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

    static const FText MaaassParticleDisplayName = LOCTEXT("EasyCrowdCategory", "MaaassParticle");
    IDetailCategoryBuilder& EasyCrowdCategory =
        DetailBuilder.EditCategory(
            TEXT("MaaassParticle"),         
            MaaassParticleDisplayName,          
            ECategoryPriority::Variable    
        );

    static const FText MPSpawnerDataAssetDisplayName = LOCTEXT("EasyCrowdAssetCategory", "MPSpawnerDataAsset");
    IDetailCategoryBuilder& EasyCrowdAssetCategory =
        DetailBuilder.EditCategory(
            TEXT("MaaassParticle|MPSpawnerDataAsset"),
            MPSpawnerDataAssetDisplayName,
            ECategoryPriority::Variable   
        );

    static const FText ComponentsDisplayName = LOCTEXT("ComponentsCategory", "Components");
    IDetailCategoryBuilder& ComponentsCategory =
        DetailBuilder.EditCategory(
            TEXT("Components"),
            ComponentsDisplayName,
            ECategoryPriority::Variable   
        );

    EasyCrowdCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, MPSpawnerDataAsset));
    EasyCrowdCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, SpawnDataGenerator));
    ComponentsCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, NiagaraComponent));


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
        EasyCrowdAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, AnimToTextureDataAsset));
        EasyCrowdAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, CrowdNiagaraSystem));
        EasyCrowdAssetCategory.AddProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, EntityConfigAsset));
    }
    else
    {
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, AnimToTextureDataAsset));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, CrowdNiagaraSystem));
        DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(AMPSpawner, EntityConfigAsset));
    }
}

#undef LOCTEXT_NAMESPACE