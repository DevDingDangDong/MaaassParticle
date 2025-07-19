// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/MPSpawner.h"
#include "IDetailCustomization.h"

/**
 * Customizes the details panel for AEasyCrowdSpawner actors.
 *
 * Provides callbacks to refresh the panel when the EasyCrowdAsset property changes,
 * retrieves the actor instance being customized, and conditionally shows or hides
 * dependent properties based on asset validity.
 */
class MAAASSPARTICLEEDITOR_API FMPSpawnerDetails : public IDetailCustomization
{
public:
	FMPSpawnerDetails();
	~FMPSpawnerDetails();

public:
	/**
	 * Creates a new instance of this detail customization class.
	 *
	 * @return A shared reference to a FEasyCrowdSpawnerDetails instance.
	 */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/**
	 * Customizes the layout of the details panel for a specific AEasyCrowdSpawner actor.
	 *
	 * Sets up a callback to refresh the panel when the EasyCrowdAsset property changes,
	 * retrieves the actor being customized, and conditionally shows or hides dependent properties.
	 *
	 * @param DetailBuilder The builder used to construct and customize the details panel layout.
	 */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TWeakObjectPtr<AMPSpawner> Spawner;
};
