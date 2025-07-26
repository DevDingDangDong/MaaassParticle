// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MPTriggerVolumeComponent.h"
#include "MPTriggerVolumeSubsystem.h"

// Sets default values for this component's properties
UMPTriggerVolumeComponent::UMPTriggerVolumeComponent()
{
	// Disable ticking to improve performance — not needed for volume checks
	PrimaryComponentTick.bCanEverTick = false;
	
	// Disable collision and ignore all channels
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);

	// Set default debug color for the volume shap
	ShapeColor = FColor::Cyan;
}

// Called when the game starts
void UMPTriggerVolumeComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		// Only register with the subsystem in actual gameplay (not editor preview)
		if (World->IsGameWorld())
		{
			if (UMPTriggerVolumeSubsystem* Subsystem = World->GetSubsystem<UMPTriggerVolumeSubsystem>())
			{
				Subsystem->RegisterComponent(this);
			}
		}
	}
	
}

void UMPTriggerVolumeComponent::OnRegister()
{
	Super::OnRegister();

	// Handle visual debug options in PIE (Play In Editor)
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		SetHiddenInGame(!bDebugVisibleInPIE);
		SetVisibility(bDebugVisibleInPIE, true);
		bDrawOnlyIfSelected = !bDebugVisibleInPIE;
	}

	// Register with the trigger subsystem
	UpdateSubsystemRegistration(true);
}

void UMPTriggerVolumeComponent::OnUnregister()
{
	// Unregister from the subsystem before destroying
	UpdateSubsystemRegistration(false);
	Super::OnUnregister();
}

void UMPTriggerVolumeComponent::UpdateSubsystemRegistration(bool bRegister)
{
	UWorld* World = this->GetWorld();
	if (World == nullptr || !World->IsGameWorld())
	{
		return;
	}

	if (UMPTriggerVolumeSubsystem* Subsystem = World->GetSubsystem<UMPTriggerVolumeSubsystem>())
	{
		if (bRegister)
		{
			Subsystem->RegisterComponent(this);
		}
		else {
			Subsystem->UnregisterComponent(this);
		}
	}
}

