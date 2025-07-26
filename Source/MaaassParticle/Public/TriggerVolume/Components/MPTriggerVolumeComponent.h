// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MPTriggerVolumeEventData.h"
#include "Components/BoxComponent.h"
#include "MPTriggerVolumeComponent.generated.h"

/**
 * A trigger volume component that can execute interaction tasks when entities enter or exit its bounds.
 *
 * This component allows users to assign Enter and Exit task lists via data assets,
 * and provides priority-based resolution when multiple volumes overlap.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAAASSPARTICLE_API UMPTriggerVolumeComponent : public UBoxComponent
{
	GENERATED_BODY()

public:	
	/** Sets default values for this component */
	UMPTriggerVolumeComponent();

protected:
	/** Called when the game starts */
	virtual void BeginPlay() override;

public:
	/** If true, the volume will be visible in PIE for debugging */
	UPROPERTY(EditAnywhere, Category = "MP TriggerVolume")
	bool bDebugVisibleInPIE = true;

	/** Event definition to apply when an entity enters this volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP TriggerVolume")
	TObjectPtr<UMPTriggerVolumeEventData> EnterInteraction;

	/** Event definition to apply when an entity exits this volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP TriggerVolume")
	TObjectPtr<UMPTriggerVolumeEventData> ExitInteraction;

	/** Priority value for resolving overlaps when multiple volumes contain an entity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MP TriggerVolume")
	int32 Priority = 0;

protected:
	/** Called when the component is registered into the world */
	virtual void OnRegister() override;

	/** Called when the component is unregistered from the world */
	virtual void OnUnregister() override;

private:
	/** Handles registration or unregistration with the interaction subsystem */
	void UpdateSubsystemRegistration(bool bRegister);
};