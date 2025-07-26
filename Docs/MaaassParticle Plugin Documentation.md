# MaaassParticle Plugin Documentation

## Navigation

[Overview](#overview)

[Features](#features)

[Planned Implementation](#planned-implementation)

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

## Planned Implementation

### Interaction Volume

We plan to provide an interaction volume component that triggers state transitions in the Mass State Tree based on gameplay tags when entities enter or exit specific areas.

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

1. Create Material for BAT Before Baking

2. Create BAT Asset

3. Configure Material for BAT After Baking

**Detail Process**

0. **Open EUW Maaass Particle Widget**
    
    <img width="227" height="418" alt="OpenEUWMaaaassParticleWidget" src="https://github.com/user-attachments/assets/9e1c7328-f487-4542-bf54-574223a7306f" />

Tools → MaaassParticle Widget

**1. Create Material for BAT Before Baking**  
    **Not Use Material Attributes**  
     - Click the red button in the Widget to copy the material nodes and paste them into the target material.  
     - Connect the Normal and WorldPositionOffset Texture value to the Input of MF_Bone Animation.  
     - Connect the output Normal and WorldPositionOffset from MF_Bone Animation to the Final Material Output.  
    
<img width="1003" height="604" alt="Image" src="https://github.com/user-attachments/assets/af7f1ad7-da5b-4c16-8e6f-0a275b7304ab" />
    
<img width="933" height="548" alt="Image" src="https://github.com/user-attachments/assets/053bb558-78d9-4bab-a434-d0e21ac4238a" />
  
   **Use Material Attributes**  
     - Click the red button in the Widget to copy the material nodes and paste them into the target material.  
     - Connect the Material Attributes Result to the Input of MF_Bone AnimationAttribute.  
     - Connect the output Material Attributes from MF_Bone AnimationAttribute to the Final Material Attributes.  
  
<img width="1003" height="604" alt="Image" src="https://github.com/user-attachments/assets/0f3e534b-70a5-4ccd-ad72-9755b13e79a7" />
<img width="960" height="407" alt="Image" src="https://github.com/user-attachments/assets/cb9d84c5-895f-4e2e-8f10-199d2a63b3c1" />

**2. Create BAT Asset**
    
Enter the information into the EUW Maaass Particle Widget and click "Create Static Mesh & AnimTextures & DataAsset.
    
<img width="1919" height="1199" alt="Image" src="https://github.com/user-attachments/assets/631069d2-2493-4447-824a-d475a2c92fe1" />

**Input Information:**
    
- **SkeletalMesh** - The source SkeletalMesh that will play the animation
- **Materials** - BAT materials created in step 1
- **Animations** - Animations to be played
- **Package Path** - Directory path where the generated resources will be saved
        
Right-click on the folder where you want to save, then copy and paste the path.
        
- **Asset Name** - Asset name to be used for the generated resources
(e.g., Asset Name: Croco ⇒ SM_Croco, DA_Croco_BAT)

  **2-1. Create BAT Result**

<img width="768" height="277" alt="Image" src="https://github.com/user-attachments/assets/64735aa6-9f5e-4916-b997-02076e1908a9" />

Texture:

- Bake the animation into Bone Animation Textures: Position, Rotation, and Weight.
- Generate the Weight textures for each LOD.

Static Mesh: 

- Convert the Skeletal Mesh to a Static Mesh.
- Apply the Material (which includes the pre-configured Bone Animation Layer) to the Static Mesh.
- In the Material's Bone Animation Layer, apply the Bone Animation Texture for each LOD.

Data Asset:

- Information about Skeletal Mesh, Static Mesh, Animation, Bone Animation Texture

**3. Configure Material for BAT After Baking**

**3-1. Material Instance Assignment by LOD**

Assign the appropriate Material Instance to the Sections of each LOD in the generated StaticMesh.

<img width="787" height="198" alt="image 5" src="https://github.com/user-attachments/assets/dd9c70c1-65f2-423f-8ef8-cc216451af35" />
<img width="784" height="187" alt="image 6" src="https://github.com/user-attachments/assets/e5036c46-c394-4a90-8e84-00201422994b" />

**3-2.** **Verify that the Layer Parameters inside the Material Instance are as follows:**

<img width="572" height="741" alt="Image" src="https://github.com/user-attachments/assets/e3d81f32-22f3-4fe7-9781-ee0e2232f41a" />

**Static Switch Parameter Value**  
By default, CrossFadeBlending and FrameBlending are only enabled for LOD0.  
If needed, you can enable them for other LODs in their respective Material Instances.  

**3-3. If using existing Material Parameters, configure them manually in the Details panel.**

<img width="574" height="597" alt="image 8" src="https://github.com/user-attachments/assets/c86f9d98-2e48-4211-b871-78f482950410" />

Ensure that the material of the generated Static Mesh has the same parameters as the material of the original Skeletal Mesh.
For example, the Texture Parameters of the new material must be configured to be identical to the original.

LOD Mode can be configured in Niagara System.

<img width="361" height="229" alt="image 9" src="https://github.com/user-attachments/assets/ace4f675-4d06-4817-89ec-a00099d6aef2" />

### Mass Spawner

**Mass Spawner Setting**  
<img width="473" height="764" alt="MPSpawner_1 1" src="https://github.com/user-attachments/assets/38ad1965-811a-4644-9c62-a1ec2cbc45ff" />

**MP Spawner Allocation**

- **MPSpawnerDataAsset**
    - **Anim to Texture Data Asset** - Data Asset containing information about the BAT asset to be used
    - **Crowd Niagara System** - Niagara system for mesh rendering (custom modules can be added if desired)
        - CPU Sim mode
    - **Entity Config Asset** - Mass Entity Data Asset for use in Mass Entity (same as Mass Entity)
    - **Default Anim State** - Default animation index num to be played
    - **Spawn Data Generator** - Settings for determining spawn location criteria for objects (same as Mass Entity)
    - **Spawn Count** - Number of entities to be spawned
    - **Loop Behavior** – Strategy for repeating entity spawn. options are **Once**, **Multiple**, **Infinite**
    - **Loop Count** – Number of times to repeat spawning. only applicable when **Loop Behavior** is set to **Multiple**
    - **Loop Duration** – Time interval (in seconds) over which each spawn loop occurs. used for timing between repeats
    - **Kill Particle On Life Has Elapsed** – If **true**, when a particle’s Age exceeds its **Particle Life Time**, both the particle and its corresponding entity are destroyed
    - **Particle Life Time** – Lifetime of a particle in seconds; only used when **Kill Particle On Life Has Elapsed** is enabled

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
There are currently no known issues.  
If you discover a problem, please report it by sending an email to: [Your Email Address]  
### Support

Contact: [devdingdangdong@gmail.com](mailto:devdingdangdong@gmail.com)

## Version History

**Version 1.0.0**

- Initial release
- Mass Entity with Niagara
- Simple BAT Asset maker tool

## License

This plugin is provided under the MIT License. See the LICENSE file for details.
