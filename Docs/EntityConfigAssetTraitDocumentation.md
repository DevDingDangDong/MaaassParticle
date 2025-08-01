<img width="1036" height="424" alt="MassEntityConfigAsset" src="https://github.com/user-attachments/assets/97d0cf75-5d06-416f-aeaf-86e8a88386a7" />

**MPMass Niagara Trait**
  - A trait that represents each entity as a Niagara particle and ensures they stay in sync.
**Assorted Fragments**
  - A catch‑all trait that provides a set of common fragments (e.g., Transform, Agent Radius) for basic entity data.
**Avoidance**
  - Enables per‑entity obstacle avoidance by computing and applying steering forces to prevent collisions.
**Movement**
  - Drives core movement logic, updating each entity’s velocity and position.
**Steering**
  - Calculates higher‑level steering behaviors and feeds them into the Movement trait.
**Smooth Orientation**
  - Interpolates an entity’s rotation over time so that turns appear fluid and natural.
**ZoneGraph Navigation**
  - Allows entities to navigate along a ZoneGraph by finding and following paths defined in navigation volumes.
**Navigation Obstacle**
  - Marks an entity as a dynamic obstacle so that navigation and avoidance systems recognize and steer around it.
**StateTree**
  - Hooks up an entity to a StateTree asset, driving complex behavior logic via a configurable state machine.


**More Information About Mass Entity**
https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-mass-entity-in-unreal-engine
