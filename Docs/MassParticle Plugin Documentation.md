# MaaassParticle Plugin Documentation

## Navigation

[Overview](https://www.notion.so/MassParticle-Plugin-Documentation-231f9aa5d19f80789afdd2d8d163bf2d?pvs=21)

[Features](https://www.notion.so/MassParticle-Plugin-Documentation-231f9aa5d19f80789afdd2d8d163bf2d?pvs=21)

[Planned Implementation](https://www.notion.so/MassParticle-Plugin-Documentation-231f9aa5d19f80789afdd2d8d163bf2d?pvs=21)

[Usage](https://www.notion.so/MassParticle-Plugin-Documentation-231f9aa5d19f80789afdd2d8d163bf2d?pvs=21)

[Notice](https://www.notion.so/MassParticle-Plugin-Documentation-231f9aa5d19f80789afdd2d8d163bf2d?pvs=21)

[Technical Details](https://www.notion.so/MassParticle-Plugin-Documentation-231f9aa5d19f80789afdd2d8d163bf2d?pvs=21)

[Troubleshooting](https://www.notion.so/MassParticle-Plugin-Documentation-231f9aa5d19f80789afdd2d8d163bf2d?pvs=21)

[Version History](https://www.notion.so/MassParticle-Plugin-Documentation-231f9aa5d19f80789afdd2d8d163bf2d?pvs=21)

## Overview

The MaaassParticle plugin

## Features

### Mass Entity + Niagara Integration

We have improved the performance of the existing Mass Entity system by implementing rendering through Niagara.

By integrating values calculated through Mass Entity—such as location, velocity, orientation, and anim state—with Niagara, we achieve real-time rendering effects.

At the same time, all existing Mass Entity functionalities remain fully available!

### BAT + LOD

By applying BAT technology to the static meshes rendered in Niagara, we enable animation playback.

We provide dynamic LOD functionality within a single static mesh, which was difficult to implement with Unreal's existing AnimToTexture system.

## Planned Implementation

### State Volume

We plan to provide State Volume functionality that allows easy modification of State or Value for entities entering the designated area.

## Installation

### Requirements

- Unreal Engine 5.6
- Windows operating system

### Installation Steps

## Usage

### Overall Usage

- BAT Static Mesh & BAT Data Asset
- Mass Data Asset & State Tree Data Asset Setup
- EC Spawner Placement and Configuration & Zone Graph Placement

### Basic Usage

### Create BAT Asset

**Overall Process**

1. Create Material for BAT Before Baking

2. Create BAT Asset

3. Configure Material for BAT After Baking

**Detail Process**

1. **Open EUW Easy Mass Particle Widget**
    
    <img width="234" height="401" alt="image" src="https://github.com/user-attachments/assets/567f4194-c794-4814-9dce-66b313476ae7" />


Tools → EMP Widget

1. **Create Material for BAT Before Baking**
    - Click the red button in the Widget to copy the material nodes and paste them into the target material.
    - Connect the Normal Texture value to the Input of MF_Bone Animation.
    - Connect the output Normal from MF_Bone Animation to the Normal input of the texture.
    - Assign the WorldPositionOffset output.
    
    <img width="824" height="494" alt="image 1" src="https://github.com/user-attachments/assets/ef330063-5d78-486d-bfb8-8a6560e2729f" />

    <img width="881" height="637" alt="image 2" src="https://github.com/user-attachments/assets/733bcce4-6a6c-4a3b-879c-662f8d658e39" />

2. **Create BAT Asset**
    
    Enter the information into the EUW Easy Crowd Widget and click "Create Static Mesh & AnimTextures & DataAsset.
    
    <img width="1002" height="727" alt="image 3" src="https://github.com/user-attachments/assets/f32efc00-49c3-4002-b4b4-6e63a43186e5" />

    **Input Information:**
    
    - **SkeletalMesh** - The source SkeletalMesh that will play the animation
    - **Materials** - BAT materials created in step 1
    - **Animations** - Animations to be played
    - **Package Path** - Directory path where the generated resources will be saved
        
        Right-click on the folder where you want to save, then copy and paste the path.
        
    - **Asset Name** - Asset name to be used for the generated resources
    (e.g., Asset Name: Croco ⇒ SM_Croco, DA_Croco_BAT)

  **2-1. Create BAT Result**

<img width="758" height="244" alt="image 4" src="https://github.com/user-attachments/assets/25204e4b-9e15-4df2-9f14-b9c3301086a5" />

Texture:

- Bake the animation into Bone Animation Textures: Position, Rotation, and Weight.
- Generate the Weight textures for each LOD.

Static Mesh: 

- Convert the Skeletal Mesh to a Static Mesh.
- Apply the Material (which includes the pre-configured Bone Animation Layer) to the Static Mesh.
- In the Material's Bone Animation Layer, apply the Bone Animation Texture for each LOD.

Data Asset:

- Information about Skeletal Mesh, Static Mesh, Animation, Bone Animation Texture

1. Configure Material for BAT After Baking

**3-1. Material Instance Assignment by LOD**

Assign the appropriate Material Instance to the Sections of each LOD in the generated StaticMesh.

<img width="787" height="198" alt="image 5" src="https://github.com/user-attachments/assets/dd9c70c1-65f2-423f-8ef8-cc216451af35" />
<img width="784" height="187" alt="image 6" src="https://github.com/user-attachments/assets/e5036c46-c394-4a90-8e84-00201422994b" />

**3-2.** **Verify that the Layer Parameters inside the Material Instance are as follows:**

<img width="705" height="755" alt="image 7" src="https://github.com/user-attachments/assets/3defbd45-4770-47f9-bc8b-5f822e11f73f" />

**Static Switch Parameter Value** 

AutoPlay: False

DynamicParameters: True

**3-3. If using existing Material Parameters, configure them manually in the Details panel.**

<img width="574" height="597" alt="image 8" src="https://github.com/user-attachments/assets/c86f9d98-2e48-4211-b871-78f482950410" />

Ensure that the material of the generated Static Mesh has the same parameters as the material of the original Skeletal Mesh.
For example, the Texture Parameters of the new material must be configured to be identical to the original.

LOD Mode can be configured in Niagara System.

<img width="361" height="229" alt="image 9" src="https://github.com/user-attachments/assets/ace4f675-4d06-4817-89ec-a00099d6aef2" />

### Mass Spawner

**Mass Spawner Setting**
<img width="479" height="840" alt="image_(6)" src="https://github.com/user-attachments/assets/e8597268-9bf9-4df4-b457-4bce436bf991" />

**Easy Crowd Asset Allocation**

- **EasyCrowdAsset**
    - DA_BAT - Data Asset containing information about the BAT asset to be used
    - Niagara System - Niagara system for mesh rendering (custom modules can be added if desired)
        - CPU Sim mode
    - Entity Config Asset - Mass Entity Data Asset for use in Mass Entity (same as Mass Entity)
- **Spawn Data Generator** - Settings for determining spawn location criteria for objects (same as Mass Entity)
- **Spawn Count** - Number of entities to be spawned
- **Spawn Interval (planned)** - Entity spawn cycle
- **Default Anim State** - Default animation to be played
- **Static Mesh** - Static Mesh to be spawned

**Particle LifetimeHandling**

<img width="641" height="331" alt="image 10" src="https://github.com/user-attachments/assets/3f6555c0-38c1-4642-9fac-937dd121618b" />

[title](https://www.notion.sohttp://)

- `Kill Particle On Lifetime` disabled : When you disable the `Kill Particles When Lifetime Has Elasped` option under **Particle Update > Particle State** in Niagara. The Entity is not destroyed based on the particle’s lifetime.
- **`Kill Particle On Lifetime`** enabled : MassEntity is destroyed when particle’s lifetime ends.

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

Plugin - MassEntity

Plugin - MassAI

## Troubleshooting

### Common Issues

<img width="1003" height="666" alt="제목_없음" src="https://github.com/user-attachments/assets/b697e156-39fa-4b9d-a1b6-d40c4f3be560" />

Verify that the Height values of the Weight Textures are identical
→ UV mapping information may become corrupted depending on the LOD level.
(Adjustable by modifying the MaxWidth in the DataAsset)

If the resulting Static Mesh appears corrupted, it is likely because the texture resolution is too low for the mesh's vertex density. To fix this, increase the Max Width and Height values and create it again.

### Support

Contact: [devdingdangdong@gmail.com](mailto:devdingdangdong@gmail.com)

## Version History

**Version 1.0.0**

- Initial release
- Mass Entity with Niagara
- Simple BAT Asset maker tool

## License

This plugin is provided under the MIT License. See the LICENSE file for details.
