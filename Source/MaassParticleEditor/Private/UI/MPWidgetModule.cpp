// Copyright 2025 DevDingDangDong, All Rights Reserved.

#include "UI/MPWidgetModule.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "LevelEditor.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "PropertyEditorModule.h"
#include "UI/MPSpawnerDetails.h"
#include "Actors/MPSpawner.h"

static const FName MPPluginName("EasyMassParticleEditor");

/**
 * Called when the module is loaded into memory.
 * Registers detail customizations and the nomad tab spawner.
 */
void MPWidgetModule::StartupModule()
{
    FPropertyEditorModule& PropEditor = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropEditor.RegisterCustomClassLayout(
        AMPSpawner::StaticClass()->GetFName(),
        FOnGetDetailCustomizationInstance::CreateStatic(&FMPSpawnerDetails::MakeInstance)
    );
    PropEditor.NotifyCustomizationModuleChanged();
    
    // Register a nomad tab spawner with the global tab manager.
    // A nomad tab can be docked anywhere in the editor.
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MPPluginName, FOnSpawnTab::CreateRaw(this, &MPWidgetModule::OnSpawnPluginTab))
        .SetDisplayName(FText::FromString(TEXT("MaaassParticle Widget"))) // The name that will appear in the Window menu.
        .SetMenuType(ETabSpawnerMenuType::Enabled) // Ensures the menu item is always visible.
        .SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports")) // Optional: An example icon.
        .SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory()); // Groups the menu item under the "Tools" category in the Window menu.
}

/**
 * Called before the module is unloaded.
 * Unregisters detail layouts and the nomad tab spawner.
 */
void MPWidgetModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
    if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
    {
        FPropertyEditorModule& PropEditor = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
        PropEditor.UnregisterCustomClassLayout(AMPSpawner::StaticClass()->GetFName());
        PropEditor.NotifyCustomizationModuleChanged();
    }

    // Unregister the tab spawner when the module is shut down to prevent memory leaks.
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MPPluginName);
}

/**
 * Spawns and returns the plugin's dockable tab.
 *
 * @param SpawnTabArgs    Arguments controlling tab creation.
 * @return A shared reference to the new SDockTab.
 */
TSharedRef<SDockTab> MPWidgetModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{    
    // 1. Define the path to the Editor Utility Widget Blueprint asset.
    const FString WidgetBlueprintPath = TEXT("/MaaassParticle/EUW_MaaassParticleWidget.EUW_MaaassParticleWidget");

    // 2. Load the Editor Utility Widget Blueprint asset from the specified path.
    UEditorUtilityWidgetBlueprint* WidgetBlueprint = LoadObject<UEditorUtilityWidgetBlueprint>(nullptr, *WidgetBlueprintPath);

    // 3. Create a new dockable tab that will host our widget.
    TSharedRef<SDockTab> NewDockTab = SNew(SDockTab)
        .TabRole(ETabRole::NomadTab);

    // 4. Check if the widget blueprint was loaded successfully.
    if (WidgetBlueprint)
    {
        // 5. Get the generated class from the blueprint, which is used to create widget instances.
        if (UWidgetBlueprintGeneratedClass* GeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(WidgetBlueprint->GeneratedClass))
        {
            // 6. Get the Editor Utility Subsystem, which manages editor utility widgets.
            UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
            if (EditorUtilitySubsystem)
            {
                // 7. Create an instance of the widget.
                UEditorUtilityWidget* CreatedWidget = CreateWidget<UEditorUtilityWidget>(GEditor->GetEditorWorldContext().World(), GeneratedClass);
                if (CreatedWidget)
                {
                    // 8. Get the underlying Slate widget from the UMG widget instance and set it as the tab's content.
                    TSharedRef<SWidget> WidgetHost = CreatedWidget->TakeWidget();
                    NewDockTab->SetContent(WidgetHost);

                    // 9. (Important) Bind a delegate to the tab's OnTabClosed event to clean up the widget.
                    //    This is the correct way to manage the widget's lifecycle, replacing the non-existent SetParentTab.
                    NewDockTab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda([EditorUtilitySubsystem, CreatedWidget](TSharedRef<SDockTab>)
                        {
                            // Release the subsystem's reference to the widget, allowing it to be garbage collected.
                            EditorUtilitySubsystem->ReleaseInstanceOfAsset(CreatedWidget);
                        }));
                }
            }
        }
    }
    // 10. (Corrected Check) If the tab's content is still the default SNullWidget, it means the widget failed to create.
    // Display an error message inside the tab.
    if (NewDockTab->GetContent() == SNullWidget::NullWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create Editor Utility Widget from path: %s"), *WidgetBlueprintPath);
        NewDockTab->SetContent(
            SNew(STextBlock)
            .Text(FText::Format(
                NSLOCTEXT("EasyMassParticle", "WidgetLoadFailed", "Failed to load or create Editor Utility Widget from path:\n{0}"),
                FText::FromString(WidgetBlueprintPath)
            ))
        );
    }

    return NewDockTab;
}

IMPLEMENT_MODULE(MPWidgetModule, MaaassParticleEditor)