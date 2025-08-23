# Creating an EntityConfigAsset
This section explains how to create and set up an **EntityConfigAsset** for use with Mass Entities. Follow the steps below:

<img width="513" height="861" alt="EntityConfigMake_0" src="https://github.com/user-attachments/assets/684c5784-423b-47ac-a49a-4d372160c9e3" />

## 1. Create a new Data Asset
- In the Content Browser, right-click on the folder where you want to create the asset.
- Select Miscellaneous → Data Asset.

<img width="620" height="178" alt="EntityConfigMake_2" src="https://github.com/user-attachments/assets/28c34ba6-c3fc-4b21-aed0-854fb96d07de" />

## 2. Search for the Asset Class
- In the pop-up window, search for **Mass Entity Config Asset**.
- Select it from the list.

<img width="1526" height="746" alt="EntityConfigMake_3" src="https://github.com/user-attachments/assets/fc5a3b0f-d6b2-4a3a-8269-c61eea63b260" />

## 3. Configure the Parent
- When prompted to assign a parent class, choose **DA_BaseMass** as the **Parent Config Asset**.

<img width="1488" height="850" alt="EntityConfigMake_4" src="https://github.com/user-attachments/assets/540b6cf6-5e02-4694-824a-f971220c59de" />

## 4. Add Required Traits and Fragments
- In the newly created asset, add the following traits and fragments:
- **Assorted Fragments**
  - A catch‑all trait that provides a set of common fragments (e.g., Transform, Agent Radius) for basic entity data.
- **ZoneGraph Navigation**
  - Allows entities to navigate along a ZoneGraph by finding and following paths defined in navigation volumes.
- **StateTree**
  - Hooks up an entity to a StateTree asset, driving complex behavior logic via a configurable state machine.
  
 <img width="884" height="694" alt="EntityConfigMake_5" src="https://github.com/user-attachments/assets/41eabea2-3295-436d-b3d5-a05e04b030f2" />

## 5. Assign a StateTree Asset
- For the **StateTree Trait**, assign the StateTree asset you want to use.
- By default, **ST_DefaultLaneFollowing** is provided. (ST_DefaultLaneFollowing allows Entities to navigate through Lanes tagged as Default.)
<img width="1919" height="934" alt="EntityConfigMake_6" src="https://github.com/user-attachments/assets/4542156c-eec8-4717-a2b1-7acbbd3afc65" />


<img width="1330" height="648" alt="EntityConfigBaseMass" src="https://github.com/user-attachments/assets/ff7ef717-ea7c-4065-8cb3-16b14c3d6d6c" />

## 6. Base Asset Reference
- The **DA_BaseMass** asset is composed of the following traits  
  
- **MPMass Niagara Trait**
  - A trait that represents each entity as a Niagara particle and ensures they stay in sync.  
- **Avoidance**
  - Enables per‑entity obstacle avoidance by computing and applying steering forces to prevent collisions.  
- **Movement**
  - Drives core movement logic, updating each entity’s velocity and position.  
- **Steering**
  - Calculates higher‑level steering behaviors and feeds them into the Movement trait.  
- **Smooth Orientation**
  - Interpolates an entity’s rotation over time so that turns appear fluid and natural.  
- **Navigation Obstacle**
  - Marks an entity as a dynamic obstacle so that navigation and avoidance systems recognize and steer around it.  

### **More Information About Mass Entity**  
https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-mass-entity-in-unreal-engine


### The plugin content already provides DA_EntityConfigAssetSample with the above setup completed. We would appreciate it if you could refer to this as a reference.

<img width="1309" height="701" alt="EntityConfigAssetSample" src="https://github.com/user-attachments/assets/4e634a44-9c35-4aff-9617-c0d5988d391c" />

