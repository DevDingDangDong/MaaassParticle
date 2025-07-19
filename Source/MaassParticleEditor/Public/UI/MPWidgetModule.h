// Copyright 2025 DevDingDangDong, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"

/**
 * Manages the MaaassParticleEditor plugin UI.
 *
 * Registers custom detail customizations and provides a dockable
 * tab hosting an Editor Utility Widget for runtime interaction.
 */
class MAAASSPARTICLEEDITOR_API MPWidgetModule : public IModuleInterface
{
public:
    /**
     * Called when the module is loaded into memory.
     *
     * Registers property editor custom layouts and the plugin's nomad tab spawner.
     */
    virtual void StartupModule() override;

    /**
     * Called before the module is unloaded.
     *
     * Unregisters custom layouts and the plugin's tab spawner to clean up.
     */
    virtual void ShutdownModule() override;

private:

    /**
     * Spawns the plugin's dockable tab containing the Editor Utility Widget.
     *
     * @param SpawnTabArgs    Arguments used to create the tab.
     * @return A shared reference to the newly created SDockTab.
     */
    TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
};
