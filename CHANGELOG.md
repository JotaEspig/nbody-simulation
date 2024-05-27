## v0.1.0 (2024-05-22)

### Feat

- throw massive objects with key X
- **script**: allowing random positions for bodies
- adding coliding detection
- **barnes-hut**: implemented node processing, now yet tested
- **celestialbodysystem**: building octree every time
- **celestialbody**: add method to calculate gravity vec using just the position
- **octree**: node method to show if it should be used or not
- **octree**: building correctly octree (I guess)
- **config.json**: add configuration file for system
- **json**: add JSON lib

### Fix

- colinding -> colliding
- sphere model
- octree contructor
- add resources from solar-system-3d

### Refactor

- includes
- changed how gravity calculation works

### Perf

- deleting bodies beyond coord 1000 or -1000
- **shaders**: removed unused stuff