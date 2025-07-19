# MassParticle Plugin Documentation

# MassParticle Plugin Documentation

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

The MassParticle plugin

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
- Windwos operating system

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
    
    ![image.png](image.png)
    

Tools → EMP Widget

1. **Create Material for BAT Before Baking**
    - Click the red button in the Widget to copy the material nodes and paste them into the target material.
    - Connect the Normal Texture value to the Input of MF_Bone Animation.
    - Connect the output Normal from MF_Bone Animation to the Normal input of the texture.
    - Assign the WorldPositionOffset output.
    
    ![image.png](image%201.png)
    
    ![image.png](image%202.png)
    
2. **Create BAT Asset**
    
    Enter the information into the EUW Easy Crowd Widget and click "Create Static Mesh & AnimTextures & DataAsset.
    
    ![image.png](image%203.png)
    
    **Input Information:**
    
    - **SkeletalMesh** - The source SkeletalMesh that will play the animation
    - **Materials** - BAT materials created in step 1
    - **Animations** - Animations to be played
    - **Package Path** - Directory path where the generated resources will be saved
        
        Right-click on the folder where you want to save, then copy and paste the path.
        
    - **Asset Name** - Asset name to be used for the generated resources
    (e.g., Asset Name: Croco ⇒ SM_Croco, DA_Croco_BAT)

  **2-1. Create BAT Result**

![image.png](image%204.png)

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

![image.png](image%205.png)

![image.png](image%206.png)

**3-2.** **Verify that the Layer Parameters inside the Material Instance are as follows:**

![image.png](image%207.png)

**Static Switch Parameter Value** 

AutoPlay: False

DynamicParameters: True

**3-3. If using existing Material Parameters, configure them manually in the Details panel.**

![image.png](image%208.png)

Ensure that the material of the generated Static Mesh has the same parameters as the material of the original Skeletal Mesh.
For example, the Texture Parameters of the new material must be configured to be identical to the original.

LOD Mode can be configured in Niagara System.

![image.png](image%209.png)

### Mass Spawner

**Mass Spawner Setting**

![image (6).png](image_(6).png)

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

![image.png](image%2010.png)

[title](https://www.notion.sohttp://)

- `Kill Particle On Lifetime` disabled : When you disable the `Kill Particles When Lifetime Has Elasped` option under **Particle Update > Particle State** in Niagara. The Entity is not destroyed based on the particle’s lifetime.
- **`Kill Particle On Lifetime`** enabled : MassEntity is destroyed when particle’s lifetime ends.

## Technical Details

### File Structure

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

### Dependencies

Plugin - AnimToTexture

Plugin - MassEntity

Plugin - MassAI

## Troubleshooting

### Common Issues

![제목 없음.png](%EC%A0%9C%EB%AA%A9_%EC%97%86%EC%9D%8C.png)

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