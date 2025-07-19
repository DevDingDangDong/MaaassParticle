// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
/**
 * Implements the editor module for EasyCrowd.
 *
 * On startup, registers detail customization for AEasyCrowdSpawner with the PropertyEditor module.
 * On shutdown, unregisters the customization if the PropertyEditor module is still loaded.
 */
class FMaaassParticleEditorModule : public IModuleInterface
{
public:

	/**
	 * Called when the module is loaded into memory.
	 *
	 * Loads the PropertyEditor module, registers the custom detail layout
	 * for AEasyCrowdSpawner, and notifies the editor that customization has changed.
	 */
	virtual void StartupModule() override;

	/**
	 * Called before the module is unloaded.
	 *
	 * If the PropertyEditor module is still loaded, unregisters the custom class layout
	 * for AEasyCrowdSpawner and notifies the editor of the change.
	 */
	virtual void ShutdownModule() override;
};
