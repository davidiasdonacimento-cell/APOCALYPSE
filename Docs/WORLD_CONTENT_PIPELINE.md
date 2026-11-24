# World Content Pipeline

## Naming and ownership

- Maps: L_ prefix, for example L_World.
- Data Layers: DL_ prefix.
- Region assets: DA_Region_<Name>.
- Region registry: DA_WorldRegionRegistry.
- POI registry: DA_WorldPOIRegistry.
- GameMode Blueprint: BP_ApocalypseGameMode.
- Environment controller Blueprint: BP_WorldEnvironmentController (parent: AWorldEnvironmentController).

## Content validation

- Region bounds must be valid and should not overlap unless explicitly documented.
- Every POI ID must be unique and non-empty.
- Every POI location must be inside its referenced region bounds.
- Keep terrain, roads, settlements, foliage, and POIs in separate Data Layers.
- Do not commit generated Saved, Intermediate, DerivedDataCache, or platform build output.

## Visual weather integration

The C++ weather state and environment lighting/fog are implemented independently of art assets. Rain streaks, wet-surface materials, thunder audio, wind-driven foliage, cloud textures, and weather Niagara systems must be authored as content assets and connected before they can be considered finished visual effects.
