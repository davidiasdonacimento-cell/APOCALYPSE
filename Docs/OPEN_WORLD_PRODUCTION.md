# APOCALYPSE — Open World Production Standard

This stage adds reusable runtime world systems. It does not claim that a finished landscape, town, forest, road network, or art package exists: those require Unreal Editor map assets and authored/imported content, not text-only source commits.

## World architecture

Use Unreal World Partition for the persistent open-world level. Do not build a custom actor-streaming system on top of it.

Required level settings:
- Enable World Partition on the persistent world.
- Use One File Per Actor for team-safe level editing.
- Configure runtime grid and streaming distance from measured traversal speed and memory budgets.
- Use HLOD layers for distant building and prop groups.
- Use Data Layers for authored states such as intact/damaged settlements, quest changes, and optional points of interest.
- Keep always-loaded actors limited to the environment controller, game-mode infrastructure, and essential managers.

## Runtime systems in this change

- UApocalypseWorldSubsystem: world clock, weather state, region lookup, and environment events.
- UWorldRegionDefinition: data-driven biome bounds, base temperature, wind, fog, and weighted weather.
- UWorldRegionRegistry: resolves a world position to its region definition.
- AWorldEnvironmentController: updates sun, sky atmosphere, skylight, and exponential height fog from environment state.
- UWorldPOIRegistry: data-driven named locations and POI classifications.

The subsystem uses world time, pauses with the game world, and broadcasts environment changes at a bounded one-second cadence rather than every frame.

## Persistent map authoring in Unreal Editor 5.8

1. Create an Open World level and save it as /Game/World/Maps/L_World.
2. Confirm World Partition is enabled in World Settings and that the map opens in World Partition Editor.
3. Create Data Layers: DL_Terrain, DL_Roads, DL_Forests, DL_Settlements, DL_Industrial, and DL_POI.
4. Build terrain with Landscape using tiled components sized for the target Android memory budget. Profile landscape material layers and texture resolution.
5. Author roads and rivers as spline-based landscape/mesh workflows; avoid thousands of independent high-cost actors.
6. Build settlements from modular building kits with collision and LODs; use HLOD for distant clusters.
7. Place foliage through the foliage system or PCG with density budgets and biome-specific species.
8. Create UWorldRegionDefinition assets with valid, preferably non-overlapping bounds and register them in one UWorldRegionRegistry asset.
9. Create one UWorldPOIRegistry asset for settlements, safe zones, hospitals, police stations, military sites, industrial sites, landmarks, and resource sites.
10. Create a Blueprint child of AApocalypseGameMode and assign the region registry asset in its defaults.
11. Place one AWorldEnvironmentController in the persistent level.
12. Set the Blueprint GameMode as the map's GameMode override and project default after the map is created.

## Mobile performance gates

- Use scalable quality levels; do not assume desktop Lumen/Nanite settings are viable on Android.
- Use baked/static lighting where appropriate, mobile-compatible materials, texture streaming, HLOD, and aggressive LODs.
- Profile CPU, GPU, draw calls, memory, and streaming hitches on the minimum target Android device.
- Set budgets before increasing landscape resolution, foliage density, shadow distance, or view distance.
- Avoid ticking individual foliage/building actors; global updates are centralized.

## Acceptance criteria

- World Partition map exists and loads without missing references.
- Streaming cells load/unload during traversal without blocking hitches beyond the agreed budget.
- Region lookup returns the correct biome at boundaries and inside each region.
- Day/night completes a full cycle at the configured duration and pauses with gameplay.
- Weather selection follows configured region weights and a world-seeded random stream.
- Environment controller responds to time, weather, and region changes.
- POI registry resolves unique IDs and authored entries reference valid regions.
- Android performance is measured on device before world-density sign-off.
