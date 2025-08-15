# MaaassParticle Plugin Documentation

## Navigation

[Overview](#overview)

[Features](#features)
[Tutorials](#tutorials)
[Usage](#usage)

[Technical Details](#technical-details)

[Troubleshooting](#troubleshooting)

[Version History](#version-history)

## Overview

The MaaassParticle plugin

## Features

### Mass Entity + Niagara Integration

We have improved the performance of the existing Mass Entity system by implementing rendering through Niagara.

By integrating values calculated through Mass Entity—such as location, velocity, orientation, and anim state—with Niagara, we achieve real-time rendering effects.

At the same time, all existing Mass Entity functionalities remain fully available!

### BAT + LOD + Blending

By applying BAT technology to the static meshes rendered in Niagara, we enable animation playback.

We provide dynamic LOD functionality within a single static mesh, which was difficult to implement with Unreal's existing AnimToTexture system.

Supports BAT-based CrossFade and Frame Blending. These can be selectively enabled or disabled per LOD level, allowing for optimization by applying animation blending only to nearby meshes.  

### Trigger Volume Interaction System

A runtime, level-based interaction event system integrated with MassStateTree is provided in the form of a component.

When an entity enters or exits a trigger volume, individual events can be sent to each entity using configurable GameplayTags.

All registered volumes are managed using a hierarchical hash grid.

At runtime, each entity's position is evaluated to detect entry or exit from volumes, and the corresponding events are dispatched to the MassStateTree system.

## Tutorials
MaaassParticle: Make BAT Tutorial
https://youtu.be/byckCPl0pgo

MaaassParticle: Make Spawner Tutorial
https://youtu.be/KUVYJl429o0

MaaassParticle: Trigger Volume Tutorial
https://youtu.be/ZxyAyNVwVhQ

## Installation

### Requirements

- Unreal Engine 5.6
- Windows operating system

### Installation Steps

## Usage

### Overall Usage

- BAT Static Mesh & BAT Data Asset
- Mass Data Asset & State Tree Data Asset Setup
- MP Spawner Placement and Configuration & Zone Graph Placement

### Basic Usage

### Create BAT Asset

**Overall Process**

1. Prepare Mesh & Material for BAT Before Baking

2. Create BAT Asset

3. Configure Material for BAT After Baking

**Detail Process**

**0. Open EUW Maaass Particle Widget**
    
<img width="227" height="418" alt="OpenEUWMaaaassParticleWidget" src="https://github.com/user-attachments/assets/9e1c7328-f487-4542-bf54-574223a7306f" />

Tools → MaaassParticle Widget  
  
**1. Set Package Path and Asset Name**  
<img width="733" height="600" alt="Image" src="https://github.com/user-attachments/assets/1922c1cc-0a1a-4d8c-ac3b-f765243a3af7" />  
By default, the Output Path is hidden until a Skeletal Mesh is assigned.  
  
<img width="732" height="602" alt="Image" src="https://github.com/user-attachments/assets/bf56825b-098d-4c07-b27e-1cc24d4cf637" />  
  
Once you assign a Skeletal Mesh Asset, the following output settings will appear. The Output Path and Asset Name are automatically populated based on the name of your selected Skeletal Mesh.  
  
- Output Path: This defines the destination directory for the baked assets.  
Default Path: It automatically defaults to a new subfolder named after your asset within the main directory: /Game/MaaassParticleBAT/[SkeletalMeshAssetName]/.  
Custom Path: To specify a different location, right-click your desired folder in the Content Browser, select Copy Path, and paste it here.  
- Asset Name: This serves as the base name for the generated files. Standard prefixes and suffixes will be added automatically.  
(e.g., an Asset Name of MyAsset will generate files like SM_MyAsset and MyAsset_BAT).
  
**Overwriting and Updating Assets**  
Using the same Output Path and Asset Name for an asset that already exists will overwrite the previous version. This is useful for iterating and live-updating assets already in use.  
Example:  
Imagine a Spawner is already placed in your world, playing a baked animation. If you re-bake the asset with a different animation and overwrite the original file, the Spawner in the world will automatically update to play the new animation.  

**Workflow Summary**  
- To update an existing asset (e.g., to use a new animation with the same Skeletal Mesh): Use the same Output Path and Asset Name.  
- To create a new, separate asset: Use a different Asset Name (e.g., MyAsset2).  

**2. Prepare Mesh & Material for BAT Before Baking**  
**2-1. Prepare Mesh & Material for BAT Before Baking (AutoMatic)**  
<img width="730" height="657" alt="Image" src="https://github.com/user-attachments/assets/21e6f09b-285b-4f19-a00f-f746fad70a95" />  

1. Drag and drop the Skeletal Mesh you want to process into the designated field.
2. Click the **Duplicate & Replace & BAT Material** button.

Clicking this button triggers the following automated sequence:

- **Creates a Directory:** A new folder named [AssetName]_BAT is created at the specified Package Path.
- **Duplicates Assets:** It creates copies of the original Skeletal Mesh and its Material, appending the _BAT suffix to their names.
- **Configures Material:** The required **Bone Animation Node** is automatically connected within the newly created _BAT material.
- **Updates UI:** The tool's UI fields are automatically updated to reference the new _BAT assets. You can verify this change by checking that the asset names in the UI now include the _BAT suffix.

**Result**  
<img width="883" height="255" alt="Image" src="https://github.com/user-attachments/assets/f46cbc31-ca47-45e2-9298-2e75d863857f" />  
If you need to bake a different animation for the **same Skeletal Mesh** later, you do not need to repeat the initial setup. Follow this simplified workflow:

1. Drag and drop the **pre-existing [AssetName]_BAT Skeletal Mesh** into the tool.
2. Change the Animation to your new target animation.
3. Proceed directly to the baking step. **Do not** click the Duplicate & Replace & BAT Material button again.

The automated setup process (Duplicate & Replace & BAT Material button) is **not compatible** with materials that use **Material Attribute Layers**. The automation will fail if such materials are present.  


**2-2. Prepare Mesh & Material for BAT Before Baking (Manual)**  

**Not Use Material Attributes**  
- Click the red button in the Widget to copy the material nodes and paste them into the target material.  
- Connect the Normal and WorldPositionOffset Texture value to the Input of MF_Bone Animation.  
- Connect the output Normal and WorldPositionOffset from MF_Bone Animation to the Final Material Output.  
<img width="730" height="53" alt="Image" src="https://github.com/user-attachments/assets/ee71bc59-0983-4add-ae3a-ad29defa3f25" />  
<img width="933" height="548" alt="Image" src="https://github.com/user-attachments/assets/053bb558-78d9-4bab-a434-d0e21ac4238a" />
  
**Use Material Attributes**  
- Click the red button in the Widget to copy the material nodes and paste them into the target material.  
- Connect the Material Attributes Result to the Input of MF_Bone AnimationAttribute.  
- Connect the output Material Attributes from MF_Bone AnimationAttribute to the Final Material Attributes.  
<img width="730" height="53" alt="Image" src="https://github.com/user-attachments/assets/3827afa1-436e-40d2-bc41-8017292948d3" />  
<img width="960" height="407" alt="Image" src="https://github.com/user-attachments/assets/cb9d84c5-895f-4e2e-8f10-199d2a63b3c1" />  
  
**3. Create BAT Asset**  

Enter the information into the EUW Maaass Particle Widget and click "Create Static Mesh & AnimTextures & DataAsset.  

**3-1. Setting Animation**  
<img width="731" height="827" alt="Image" src="https://github.com/user-attachments/assets/4e2b0615-628e-4443-a79d-17b273aa5407" />  
- Is Loop : When checked, the animation will play in a continuous loop.
  
**3-2. Setting AnimToTexture Info**  
<img width="731" height="827" alt="Image" src="https://github.com/user-attachments/assets/f59e37ff-3c6c-4c05-b121-20d182e057f8" />  
  
- Num Driver Triangles: The number of driver triangles used for skinning, affecting the detail of the deformation.  
- Sigma: A smoothing factor for the skinning weights; higher values result in smoother, more blended deformations.  
- Enforce Power of Two: Forces the output texture dimensions to the nearest power of two, which can improve performance and compatibility.  
- Precision: Sets the bit depth of the texture data (e.g., 8-bit or 16-bit), trading between memory usage and deformation accuracy.  
- Num Bone Influences: The maximum number of bones that can affect a single vertex, balancing performance against animation complexity.  
  
**If you experience flickering or jittering in the animation, increase the Precision setting and bake the animation again.**  
  
**3-3. Create BAT Result**

<img width="1154" height="251" alt="Image" src="https://github.com/user-attachments/assets/876135b5-bbe9-49df-9225-c1386fb95506" />

Texture:

- Bake the animation into Bone Animation Textures: Position, Rotation, and Weight.
- Generate the Weight textures for each LOD.

Static Mesh: 

- Convert the Skeletal Mesh to a Static Mesh.
- Apply the Material (which includes the pre-configured Bone Animation Layer) to the Static Mesh.
- In the Material's Bone Animation Layer, apply the Bone Animation Texture for each LOD.

Data Asset:

- Information about Skeletal Mesh, Static Mesh, Animation, Bone Animation Texture
  
**4. Configure Material for BAT After Baking**

**4-1. Material Instance Assignment by LOD**

Assign the appropriate Material Instance to the Sections of each LOD in the generated StaticMesh.

<img width="787" height="198" alt="image 5" src="https://github.com/user-attachments/assets/dd9c70c1-65f2-423f-8ef8-cc216451af35" />
<img width="784" height="187" alt="image 6" src="https://github.com/user-attachments/assets/e5036c46-c394-4a90-8e84-00201422994b" />

**4-2.** **Verify that the Layer Parameters inside the Material Instance are as follows:**

<img width="572" height="741" alt="Image" src="https://github.com/user-attachments/assets/e3d81f32-22f3-4fe7-9781-ee0e2232f41a" />

**Static Switch Parameter Value**  
By default, CrossFadeBlending and FrameBlending are only enabled for LOD0.  
If needed, you can enable them for other LODs in their respective Material Instances.  

**4-3. If using existing Material Parameters, configure them manually in the Details panel.**

<img width="574" height="597" alt="image 8" src="https://github.com/user-attachments/assets/c86f9d98-2e48-4211-b871-78f482950410" />

Ensure that the material of the generated Static Mesh has the same parameters as the material of the original Skeletal Mesh.
For example, the Texture Parameters of the new material must be configured to be identical to the original.

LOD Mode can be configured in Niagara System.

<img width="361" height="229" alt="image 9" src="https://github.com/user-attachments/assets/ace4f675-4d06-4817-89ec-a00099d6aef2" />

### Mass Spawner

**Mass Spawner Setting**  
<img width="554" height="759" alt="MPSpawner" src="https://github.com/user-attachments/assets/3d890ca9-8b83-4d3e-a0d6-686d7aaf6cc8" />


**MP Spawner Allocation**

- **MPSpawnerDataAsset**
    - **Anim to Texture Data Asset** - Data Asset containing information about the BAT asset to be used
    - **Crowd Niagara System** - Niagara system for mesh rendering (custom modules can be added if desired)
        - CPU Sim mode
        - the Niagara Systems we provide
            - **NS_MaaassParticle_Once** : Spawns a one-time batch of Entities equal to the configured SpawnCount.
            - **NS_MaaassParticle_Once_KillAboutLifetime** : Spawns a one-time batch of Entities equal to the configured SpawnCount. Each Entity remains alive for ParticleLifetime seconds, then is automatically destroyed.
            - **NS_MaaassParticle_Multiple** : Spawns batches of Entities equal to SpawnCount repeatedly for LoopCount iterations, with LoopDuration seconds between each spawn.
            - **NS_MaaassParticle_Multiple_KillAboutLifetime** : Spawns batches of Entities equal to SpawnCount repeatedly for LoopCount iterations, with LoopDuration seconds between each spawn. Each Entity persists for ParticleLifetime seconds before being destroyed.
            - **NS_MaaassParticle_Infinite** : Continuously spawns batches of Entities equal to SpawnCount at intervals of LoopDuration seconds, with no limit on the number of spawns.
            - **NS_MaaassParticle_Infinite_KillAboutLifetime** : Continuously spawns batches of Entities equal to SpawnCount at intervals of LoopDuration seconds. Each Entity persists for ParticleLifetime seconds before being destroyed.
    - **Entity Config Asset** - Mass Entity Data Asset for use in Mass Entity (same as Mass Entity)
    - **Default Anim State** - Default animation index num to be played
    - **Spawn Data Generator** - Settings for determining spawn location criteria for objects (same as Mass Entity)
    - **Spawn Count** - Number of entities to be spawned
    - **Loop Count** – Number of times to repeat spawning. only applicable when **Crowd Niagara System** is set to **NS_MaaassParticle_Multiple** and **NS_MaaassParticle_Multiple_KillAboutLifetime**
    - **Loop Duration** – Time interval (in seconds) over which each spawn loop occurs. used for timing between repeats
    - **Particle Life Time** – Lifetime of a particle in seconds; only used for Niagara Systems with the _KillAboutLifetime suffix.

### Usage of Trigger Volume.
**Overall Process**

1. Add a `UMPTriggerVolumeComponent` to the actor you want to use as a trigger volume.

2. Create a Trigger Volume Data Asset and assign tasks for enter and exit events.

3. Configure a MassStateTree to handle the corresponding events.

**Detail Process**

1. **Add a `UMPTriggerVolumeComponent` to the actor.**  
Add a `UMPTriggerVolumeComponent` to the actor you want to use as a trigger volume.  
<img width="511" height="159" alt="image" src="https://github.com/user-attachments/assets/7259003c-14c7-403b-a992-51f7ab0b7872" />

2. **Create a Trigger Volume Data Asset.**  
   You can assign two types of events to a trigger volume: enter and exit.  
<img width="209" height="170" alt="image" src="https://github.com/user-attachments/assets/51cad004-1ec6-4800-bfae-c546a12d2f4e" />

2-1. **Create Data Asset.**  
Create separate MP Trigger Volume Event Data assets for the enter and exit events.  
<img width="618" height="177" alt="image" src="https://github.com/user-attachments/assets/0c01b780-09f2-475d-af31-59783edbc6c5" />

2-2. **Data Asset Setting**  
Add new elements to the Trigger Volume Tasks array and assign an MPRequestEventTask.
In the Event To Send field, set the Gameplay Tag that identifies the event.  
<img width="1339" height="450" alt="image" src="https://github.com/user-attachments/assets/0a0eec35-0518-49e6-a512-be5a281d32a5" />

2-3. **Trigger Volume Component Configuaration**  
Assign the created data assets to the Enter Interaction and Exit Interaction fields of the component.  
The meaning of each property is as follows:  

- Debug Visible in PIE: Whether to render the bounding box during Play In Editor (PIE) mode.  

- Enter Interaction: The event to execute when an entity enters the volume.  

- Exit Interaction: The event to execute when an entity exits the volume.  

- Priority: Determines the execution priority when multiple trigger volumes overlap.
(Only one trigger volume’s tasks will be executed per entity at a time.)  
<img width="510" height="469" alt="image" src="https://github.com/user-attachments/assets/32bf691c-5908-4453-9565-540c39dab8b4" />

5. **Configure a MassStateTree**  
Let’s set up the Mass StateTree to handle the actual events and trigger state transitions.  
<img width="98" height="169" alt="image" src="https://github.com/user-attachments/assets/7f26f2df-7e70-4fcb-8d54-0862f05cf76e" />

5-1. **Create Mass State Tree**  
Create State Tree for Mass Behaviors.  
<img width="619" height="169" alt="image" src="https://github.com/user-attachments/assets/ee5f6d33-5c5b-401a-b256-b1c3e6f43959" />

5-2. **Create Mass State Tree Node**  
Create InVolume and OutVolume nodes that contain the tasks to execute when enter or exit events are received.  
<img width="662" height="257" alt="image" src="https://github.com/user-attachments/assets/33d23878-6efe-4dc1-b21c-54b61e7013de" />

5-3. **Basic ZoneGraph-based movement Task Setup**  
Set up basic ZoneGraph-based movement.  
<img width="579" height="238" alt="image" src="https://github.com/user-attachments/assets/71392adf-045c-4f8c-bf90-d641b0fd7df3" />

5-4. **Configure Transition**  
In the node that receives the event (e.g., Idle), configure the transitions related to the event.  
<img width="568" height="690" alt="image" src="https://github.com/user-attachments/assets/34f4693f-9730-4735-8955-d41e201e6a90" />

5-5. **Define Event-Related Tasks**  
Define the tasks to be executed when an event is received. In this case, they include animation transitions and debug enter/exit logs.
* MP Set Anim State : A task that passes a new animation index via a fragment to update the animation rendered through Niagara.
* MP Debug Log Task : A task that prints debug output on entering and exiting the node.  
<img width="575" height="409" alt="image" src="https://github.com/user-attachments/assets/89b146be-d379-4fd6-a198-3d94abfe2be2" />

5-6. **Complete**  
<img width="836" height="520" alt="image" src="https://github.com/user-attachments/assets/5e8d932e-e19b-4537-8f6e-5fc252c90292" />

## Technical Details

### File Structure

```
MaaassParticle
├── Config
│   └── FilterPlugin.ini
├── Resource
│   └── Icon128.png
└── Source
├── MaaassParticle
│   ├── Actor
│   │   └── MPSpawner.h
│   ├── MassEntity
│   │   ├── Fragments
│   │   │   ├── MPAnimStateFragment.h
│   │   │   └── ...
│   │   ├── Processors
│   │   │   ├── MPAnimStateSyncProcessor.h
│   │   │   └── ...
│   │   ├── Tasks
│   │   │   └── MPSetAnimStateTask.h
│   │   └── Traits
│   │       └── MPMassNiagaraTrait.h
│   ├── NiagaraDataInterface
│   │   ├── NiagaraDataInterfaceLODBAT.h
│   │   └── NiagaraDataInterfaceMassEntity.h
│   ├── MPSpawnerDataAsset.h
│   └── MPParticleLifeCycleSubsystem.h
└── MaaassParticleEditor
├── AnimToTexture
│   └── MPAnimToTextureBPLibrary.h
└── UI
├── MPEditorModule.h
└── MPSpawnerDetails.h
```

### Dependencies

Plugin - AnimToTexture

Plugin - MassAI

Plugin - MassCrowd

Plugin - MassGamePlay

## Troubleshooting

### Common Issues
### 1. Material Apply Problem
<img width="812" height="509" alt="Apply" src="https://github.com/user-attachments/assets/3b3bf72d-69ae-4500-87e1-c7eb97c761c7" />  
  
If a generated Static Mesh is not displaying its bone animation, you may need to manually re-apply its Material.  
  
1.  Open the **Material** that contains the `Bone Animation Material` node.
2.  Click the **Apply** button.
3.  If the `Apply` button is disabled, simply move any node slightly to activate it, and then click **Apply**.
  
This will force the Static Mesh to update and render the animation correctly. This issue may occur with certain assets.  

### 2. AnimToTextureDataAsset Null Problem
<img width="1356" height="925" alt="TroubleShooting_AnimToTextureDataAsset_Null_Error" src="https://github.com/user-attachments/assets/84945a39-ba4e-4348-9018-7061e78cd121" />

If, upon entering PIE, each Entity appears distorted or remains static (as shown above), and the Output Log shows the error **`AnimToTextureDataAsset is null`**, follow the steps below to resolve it.

  
<img width="556" height="647" alt="TrobuleShooting_2_Solution_0" src="https://github.com/user-attachments/assets/38143d62-769e-42de-b883-20e08677633e" />

1. Open the **Details** panel of the **MPSpawner**.
  
<img width="558" height="824" alt="TrobuleShooting_2_Solution_1" src="https://github.com/user-attachments/assets/e03236b9-f05c-4f19-9ec0-b5a7e1299cae" />

2. In the MPSpawner hierarchy, select the **NiagaraComponent** at the bottom.
3. In the **Details** panel, locate the **LODBAT** section under **User Parameters**.
4. **Reset** the **Animation To Texture Data Asset** field (click the arrow icon on the right to reset).
  
<img width="545" height="782" alt="TrobuleShooting_2_Solution_2" src="https://github.com/user-attachments/assets/674568fc-2265-4871-95b7-d6551f380ac9" />

5. Choose the appropriate **Animation To Texture Data Asset** to apply to the Entities you want to spawn.
  
After completing these steps, spawned Entities should play their animations correctly.
This issue may vary depending on the current machine environment; we will identify the root cause and provide a fix as soon as possible.
  
If you discover another problem, please report it by sending an email to: [devdingdangdong@gmail.com](mailto:devdingdangdong@gmail.com)
  
### Support

Contact: [devdingdangdong@gmail.com](mailto:devdingdangdong@gmail.com)

## Version History

**Version 1.0.0**

- Initial release
- Mass Entity with Niagara
- Simple BAT Asset maker tool

## License

This plugin is provided under the MIT License. See the LICENSE file for details.
