# APOCALYPSE — Inventory, Loot and Storage

## Systems delivered
- Data-driven item catalog with stable IDs, display names, categories, stack limits and consumable effects.
- Replicated player/container inventory with configurable slot capacity, stack merging, partial acceptance and authoritative add/remove operations.
- Consumable use connected to the existing survival component (food, water and health restoration).
- Replicated world pickups that transfer only accepted quantities and remain while a remainder exists.
- Harvestable resource nodes with configurable yield and remaining quantity.
- Storage container actors with their own inventory component and a server-side open event.
- Server-authoritative transfers between inventories in the same world and within 500 Unreal units.

## Editor setup
1. Create a UApocalypseItemCatalog Data Asset.
2. Add unique, non-empty item IDs. Set stack limits from 1 to 999 and configure restoration values for consumables.
3. Assign the catalog to the player's inventory and each storage inventory.
4. Place pickup, resource-node and storage-container actors in the level and configure their item IDs and quantities.
5. Bind OnStorageOpened to the inventory UI. Show the storage inventory beside the player's inventory.
6. Call RequestTransferTo on the source inventory to move a selected quantity. The server revalidates world, authority, quantity and distance.

## Rules and invariants
- Unknown IDs and invalid quantities never enter an inventory.
- AddItem returns the actual accepted quantity, supporting partial pickup/harvest when full.
- Existing stacks are filled before new slots are allocated.
- Removal is all-or-nothing.
- Inventory mutations and item consumption run on the server.
- Replicated inventory arrays notify clients through OnInventoryChanged.

## Integration boundaries
This is a functional C++ gameplay backend, not a finished inventory HUD or authored loot table. Visual meshes, pickup prompts, storage UI, item icons, Data Assets and level placement must be authored in Unreal Editor. No placeholder UI is represented as finished content.

## Acceptance checks
- Unknown item IDs, invalid quantities and client-side direct mutations are rejected.
- Stack and slot limits are respected.
- Full inventories preserve uncollected pickup/resource quantities.
- Consuming an item removes one unit and applies its configured survival effects.
- Storage transfers cannot exceed source quantity or destination capacity.
- Multiplayer inventory state replicates from server-owned actors/components.
