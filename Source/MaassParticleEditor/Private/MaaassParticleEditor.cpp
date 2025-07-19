// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "MaaassParticleEditor.h"

#define LOCTEXT_NAMESPACE "FMaaassParticleModuleEditor"

/**
 * Registers the detail customization for AEasyCrowdSpawner with the PropertyEditor module.
 */
void FMaaassParticleEditorModule::StartupModule()
{
	
}

/**
 * Unregisters the detail customization if the PropertyEditor module is still loaded.
 */
void FMaaassParticleEditorModule::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMaaassParticleEditorModule, MaaassParticle)